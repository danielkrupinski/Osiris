#include <Windows.h>
#include <Psapi.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "Config.h"
#include "GUI.h"
#include "Hacks/Misc.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "Memory.h"
#include "SDK/UserCmd.h"
#include "Hacks/Glow.h"
#include "Hacks/Triggerbot.h"
#include "Hacks/Chams.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT __stdcall hookedWndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (GetAsyncKeyState(VK_INSERT) & 1)
        gui.isOpen = !gui.isOpen;

    if (gui.isOpen && !ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
        return true;

    return CallWindowProc(hooks.originalWndProc, window, msg, wParam, lParam);
}

static HRESULT __stdcall hookedPresent(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
{
    static bool isInitialised{ false };

    if (!isInitialised) {
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(FindWindowA("Valve001", NULL));
        ImGui_ImplDX9_Init(device);

        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.WindowBorderSize = 0.0f;
        style.ChildBorderSize = 0.0f;
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        char buffer[MAX_PATH];
        GetWindowsDirectoryA(buffer, MAX_PATH);
        io.Fonts->AddFontFromFileTTF(std::string{ buffer + std::string{ "\\Fonts\\Tahoma.ttf" } }.c_str(), 16.0f);

        hooks.originalWndProc = reinterpret_cast<WNDPROC>(
            SetWindowLongPtr(FindWindowA("Valve001", NULL), GWLP_WNDPROC, LONG_PTR(hookedWndProc))
            );
        isInitialised = true;
    }
    else if (gui.isOpen) {
        device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
        IDirect3DVertexDeclaration9* vertexDeclaration;
        device->GetVertexDeclaration(&vertexDeclaration);

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        gui.render();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

        device->SetVertexDeclaration(vertexDeclaration);
    }
    return hooks.originalPresent(device, src, dest, windowOverride, dirtyRegion);
}

static HRESULT __stdcall hookedReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    auto result = hooks.originalReset(device, params);
    ImGui_ImplDX9_CreateDeviceObjects();
    return result;
}

static bool __stdcall hookedCreateMove(float inputSampleTime, UserCmd* cmd) noexcept
{
    hooks.clientMode.getOriginal<void(__thiscall*)(ClientMode*, float, UserCmd*)>(24)(memory.clientMode, inputSampleTime, cmd);
    if (interfaces.engineClient->IsConnected() && interfaces.engineClient->IsInGame()) {
        Misc::skybox();
        Misc::bunnyHop(cmd);
        Misc::removeCrouchCooldown(cmd);
        Misc::clanTag();
        Triggerbot::run(cmd);
    }
    return false;
}

static void __stdcall hookedLockCursor() noexcept
{
    if (gui.isOpen)
        interfaces.surface->unlockCursor();
    else
        hooks.surface.getOriginal<void(__thiscall*)(Surface*)>(67)(interfaces.surface);
}

static int __stdcall hookedDoPostScreenEffects(int param) noexcept
{
    if (interfaces.engineClient->IsConnected() && interfaces.engineClient->IsInGame()) {
        Misc::inverseRagdollGravity();
        Misc::removeBlood();
        Misc::removeSmoke();
        Misc::reduceFlashEffect();
        Misc::disablePostProcessing();
        Misc::colorWorld();
        Glow::render();
        Chams::render();
    }
    return hooks.clientMode.getOriginal<int(__thiscall*)(ClientMode*, int)>(44)(memory.clientMode, param);
}

static float __stdcall hookedGetViewModelFov() noexcept
{
    if (interfaces.engineClient->IsConnected() && interfaces.engineClient->IsInGame() && !(*memory.localPlayer)->isScoped())
        return static_cast<float>(config.misc.viewmodelFov);
    else
        return 60.0f;
}

Hooks::Hooks()
{
    originalPresent = **reinterpret_cast<decltype(&originalPresent)*>(memory.present);
    **reinterpret_cast<void***>(memory.present) = reinterpret_cast<void*>(&hookedPresent);
    originalReset = **reinterpret_cast<decltype(&originalReset)*>(memory.reset);
    **reinterpret_cast<void***>(memory.reset) = reinterpret_cast<void*>(&hookedReset);

    surface.hookAt(67, hookedLockCursor);

    clientMode.hookAt(24, hookedCreateMove);
    clientMode.hookAt(44, hookedDoPostScreenEffects);
    clientMode.hookAt(35, hookedGetViewModelFov);
}

Hooks::Vmt::Vmt(void* const base)
{
    oldVmt = *reinterpret_cast<std::uintptr_t**>(base);
    length = calculateLength(oldVmt);

    newVmt = findFreeDataPage_2(base, length);
    std::copy(oldVmt, oldVmt + length, newVmt);
    *reinterpret_cast<std::uintptr_t**>(base) = newVmt;
}

std::uintptr_t* Hooks::Vmt::findFreeDataPage(void* const base, std::size_t vmtSize)
{
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(base, &mbi, sizeof(mbi));
    MODULEINFO moduleInfo;
    GetModuleInformation(GetCurrentProcess(), static_cast<HMODULE>(mbi.AllocationBase), &moduleInfo, sizeof(moduleInfo));
    const auto module_end = reinterpret_cast<std::uintptr_t>(moduleInfo.lpBaseOfDll) + moduleInfo.SizeOfImage - sizeof(std::uintptr_t);

    for (auto current_address = module_end; current_address > (DWORD)moduleInfo.lpBaseOfDll; current_address -= sizeof(std::uintptr_t)) {
        if (*reinterpret_cast<std::uintptr_t*>(current_address) == 0
            && VirtualQuery(reinterpret_cast<LPCVOID>(current_address), &mbi, sizeof(mbi))
            && mbi.State == MEM_COMMIT && mbi.Protect == PAGE_READWRITE && mbi.RegionSize >= vmtSize) {
            bool is_good_vmt = true;
            auto page_count = 0u;

            for (; page_count < vmtSize && is_good_vmt; page_count += sizeof(std::uintptr_t)) {
                if (*reinterpret_cast<std::uintptr_t*>(current_address + page_count) != 0)
                    is_good_vmt = false;
            }

            if (is_good_vmt && page_count >= vmtSize)
                return (uintptr_t*)current_address;
        }
    }
    return nullptr;
}

std::uintptr_t* Hooks::Vmt::findFreeDataPage_2(void* const base, std::size_t vmtSize)
{
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(base, &mbi, sizeof(mbi));
    MODULEINFO moduleInfo;
    GetModuleInformation(GetCurrentProcess(), static_cast<HMODULE>(mbi.AllocationBase), &moduleInfo, sizeof(moduleInfo));

    std::uintptr_t* moduleEnd{ reinterpret_cast<std::uintptr_t*>(static_cast<std::byte*>(moduleInfo.lpBaseOfDll) + moduleInfo.SizeOfImage) };

    for (auto currentAddress = moduleEnd - vmtSize; currentAddress > moduleInfo.lpBaseOfDll; currentAddress--)
        if (!*currentAddress) {
            if (VirtualQuery(currentAddress, &mbi, sizeof(mbi)) && mbi.State == MEM_COMMIT
                && mbi.Protect == PAGE_READWRITE && mbi.RegionSize >= vmtSize * sizeof(std::uintptr_t)
                && std::all_of(currentAddress, currentAddress + vmtSize, [](std::uintptr_t a) { return !a; }))
                return currentAddress;
        }
        else
            currentAddress -= vmtSize;
}

std::size_t Hooks::Vmt::calculateLength(std::uintptr_t* vmt) const noexcept
{
    std::size_t length{ 0 };
    MEMORY_BASIC_INFORMATION memoryInfo;
    while (VirtualQuery(reinterpret_cast<LPCVOID>(vmt[length]), &memoryInfo, sizeof(memoryInfo)) && memoryInfo.Protect == PAGE_EXECUTE_READ)
        length++;
    return length;
}
