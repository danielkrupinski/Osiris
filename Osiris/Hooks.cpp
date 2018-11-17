#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Psapi.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "xorstr.hpp"

#include "Config.h"
#include "GUI.h"
#include "Hacks/Misc.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "Memory.h"
#include "SDK/UserCmd.h"
#include "Hacks/Glow.h"
#include "Hacks/Triggerbot.h"
#include <vector>
#include <fstream>
#include "Hacks/Chams.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT __stdcall hookedWndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (GetAsyncKeyState(VK_INSERT) & 1)
        gui.isOpen = !gui.isOpen;

    if (gui.isOpen && !ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
        return true;

    return CallWindowProc(hooks.originalWndProc, window, msg, wParam, lParam);
}

static HRESULT __stdcall hookedPresent(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion)
{
    static bool isInitialised{ false };

    if (!isInitialised) {
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(FindWindowA(xorstr_("Valve001"), NULL));
        ImGui_ImplDX9_Init(device);

        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.WindowBorderSize = 0.0f;
        style.ChildBorderSize = 0.0f;
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;

        hooks.originalWndProc = reinterpret_cast<WNDPROC>(
            SetWindowLongPtr(FindWindowA(xorstr_("Valve001"), NULL), GWLP_WNDPROC, LONG_PTR(hookedWndProc))
            );
        isInitialised = true;
    }
    else if (gui.isOpen) {
        DWORD d3rsColorWrite;
        device->GetRenderState(D3DRS_COLORWRITEENABLE, &d3rsColorWrite);
        IDirect3DVertexDeclaration9* vertexDeclaration;
        device->GetVertexDeclaration(&vertexDeclaration);
        IDirect3DVertexShader9* vertexShader;
        device->GetVertexShader(&vertexShader);
        device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
        device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
        device->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        device->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        device->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
        device->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        gui.render();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

        device->SetRenderState(D3DRS_COLORWRITEENABLE, d3rsColorWrite);
        device->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
        device->SetVertexDeclaration(vertexDeclaration);
        device->SetVertexShader(vertexShader);
    }
    Misc::colorWorld();
    return hooks.originalPresent(device, src, dest, windowOverride, dirtyRegion);
}

static HRESULT __stdcall hookedReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params)
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    auto result = hooks.originalReset(device, params);
    ImGui_ImplDX9_CreateDeviceObjects();
    return result;
}

static bool __fastcall hookedCreateMove(void*, void*, float inputSampleTime, UserCmd* cmd)
{
    hooks.clientMode.getOriginal<void(__thiscall*)(ClientMode*, float, UserCmd*)>(24)(memory.clientMode, inputSampleTime, cmd);
    if (interfaces.engineClient->IsConnected() && interfaces.engineClient->IsInGame()) {
        Misc::skybox();
        Misc::bunnyHop(cmd);
        Misc::clanTag();
        Triggerbot::run(cmd);
    }
    return false;
}

static void __fastcall hookedLockCursor(Surface* thisptr, void* edx)
{
    if (gui.isOpen)
        interfaces.surface->UnlockCursor();
    else
        hooks.surface.getOriginal<void(__fastcall*)(Surface*, void*)>(67)(thisptr, edx);
}

static int __stdcall hookedDoPostScreenEffects(int param)
{
    if (interfaces.engineClient->IsConnected() && interfaces.engineClient->IsInGame()) {
        Misc::removeSmoke();
        Misc::reduceFlashEffect();
        Misc::disablePostProcessing();
        Glow::render();
        Chams::render();
    }
    return hooks.clientMode.getOriginal<int(__thiscall*)(ClientMode*, int)>(44)(memory.clientMode, param);
}

static float __stdcall hookedGetViewModelFov()
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
    surface.apply();

    clientMode.hookAt(24, hookedCreateMove);
    clientMode.hookAt(44, hookedDoPostScreenEffects);
    clientMode.hookAt(35, hookedGetViewModelFov);
    clientMode.apply();
}

Hooks::Vmt::Vmt(void* base)
{
    std::string moduleName;

    classBase = base;
    oldVmt = *reinterpret_cast<std::uintptr_t**>(base);
    vmtLength = calculateLength(oldVmt);

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(base, &mbi, sizeof(mbi))) {
        char buffer[MAX_PATH];
        GetModuleFileNameA((HMODULE)mbi.AllocationBase, buffer, MAX_PATH);
        std::string temp = buffer;
        const size_t last_slash_idx = temp.find_last_of("\\/");
        if (std::string::npos != last_slash_idx)
            temp.erase(0, last_slash_idx + 1), moduleName = temp;
    }
    if (!moduleName.empty()) {
        newVmt = findFreeDataPage(moduleName, vmtLength * sizeof(std::uintptr_t));
        std::memset(newVmt, NULL, vmtLength * sizeof(std::uintptr_t));
        std::memcpy(newVmt, oldVmt, vmtLength * sizeof(std::uintptr_t));
    }
}

void Hooks::Vmt::apply()
{
    *reinterpret_cast<std::uintptr_t**>(classBase) = newVmt;
}

std::uintptr_t* Hooks::Vmt::findFreeDataPage(const std::string& module, std::size_t vmtSize)
{
    auto check_data_section = [&](LPCVOID address, const std::size_t vmt_size)
    {
        constexpr auto dataProtection = PAGE_EXECUTE_READWRITE | PAGE_READWRITE;
        MEMORY_BASIC_INFORMATION mbi;

        if (VirtualQuery(address, &mbi, sizeof(mbi)) == sizeof(mbi) && mbi.AllocationBase && mbi.BaseAddress &&
            mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_GUARD) && mbi.Protect != PAGE_NOACCESS)
        {
            if (mbi.Protect & dataProtection && mbi.RegionSize >= vmt_size)
            {
                return mbi.Protect & dataProtection && mbi.RegionSize >= vmt_size ? true : false;
            }
        }
        return false;
    };

    MODULEINFO moduleInfo;
    GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module.c_str()), &moduleInfo, sizeof(moduleInfo));
    const auto module_end = reinterpret_cast<std::uintptr_t>(moduleInfo.lpBaseOfDll) + moduleInfo.SizeOfImage - sizeof(std::uintptr_t);

    for (auto current_address = module_end; current_address > (DWORD)moduleInfo.lpBaseOfDll; current_address -= sizeof(std::uintptr_t))
    {
        if (*reinterpret_cast<std::uintptr_t*>(current_address) == 0 && check_data_section(reinterpret_cast<LPCVOID>(current_address), vmtSize))
        {
            bool is_good_vmt = true;
            auto page_count = 0u;

            for (; page_count < vmtSize && is_good_vmt; page_count += sizeof(std::uintptr_t))
            {
                if (*reinterpret_cast<std::uintptr_t*>(current_address + page_count) != 0)
                    is_good_vmt = false;
            }

            if (is_good_vmt && page_count >= vmtSize)
                return (uintptr_t*)current_address;
        }
    }
    return nullptr;
}

std::size_t Hooks::Vmt::calculateLength(std::uintptr_t* vmt) const
{
	std::size_t length{ 0 };
	MEMORY_BASIC_INFORMATION memoryInfo;
	while (VirtualQuery(reinterpret_cast<LPCVOID>(vmt[length]), &memoryInfo, sizeof(memoryInfo)) && memoryInfo.Protect == PAGE_EXECUTE_READ)
		length++;
	return length;
}
