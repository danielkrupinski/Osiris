#include <intrin.h>
#include <Windows.h>
#include <Psapi.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "Config.h"
#include "GUI.h"
#include "Hacks/Misc.h"
#include "Hacks/Aimbot.h"
#include "Hacks/Visuals.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "Memory.h"
#include "SDK/UserCmd.h"
#include "SDK/ModelRender.h"
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

    static bool isMenuToggled{ false };

    if (!isInitialised) {
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(FindWindowA("Valve001", NULL));
        ImGui_ImplDX9_Init(device);

        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 5.0f;
        style.WindowBorderSize = 0.0f;
        style.ChildBorderSize = 0.0f;
        style.GrabMinSize = 7.0f;
        style.GrabRounding = 5.0f;
        style.FrameRounding = 5.0f;
        style.PopupRounding = 5.0f;
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        io.MouseDrawCursor = true;
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

        if (!isMenuToggled) {
            memory.input->isMouseInitialized = false;
            memory.input->isMouseActive = false;
            interfaces.inputSystem->enableInput(false);
            isMenuToggled = true;
        }
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        gui.render();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

        device->SetVertexDeclaration(vertexDeclaration);
    }
    else {
        if (isMenuToggled) {
            memory.input->isMouseInitialized = true;
            interfaces.inputSystem->enableInput(true);
            interfaces.inputSystem->resetInputState();
            isMenuToggled = false;
        }
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
    if (interfaces.engine->isInGame()) {
        Misc::skybox();
        Misc::bunnyHop(cmd);
        Misc::removeCrouchCooldown(cmd);
        Aimbot::run(cmd);
        Triggerbot::run(cmd);
        Misc::autoPistol(cmd);
        Misc::animateClanTag();
        cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
        cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
        cmd->viewangles.z = 0.0f;
    }
    return false;
}

static int __stdcall hookedDoPostScreenEffects(int param) noexcept
{
    if (interfaces.engine->isInGame()) {
        Visuals::modifySmoke();
        Visuals::thirdperson();
        Misc::inverseRagdollGravity();
        Visuals::reduceFlashEffect();
        Misc::disablePostProcessing();
        Misc::colorWorld();
        Glow::render();
    }
    return hooks.clientMode.getOriginal<int(__thiscall*)(ClientMode*, int)>(44)(memory.clientMode, param);
}

static float __stdcall hookedGetViewModelFov() noexcept
{
    return hooks.clientMode.getOriginal<float(__thiscall*)(ClientMode*)>(35)(memory.clientMode) + static_cast<float>(config.visuals.viewmodelFov);
}

static void __stdcall hookedDrawModelExecute(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    if (interfaces.engine->isInGame() && !interfaces.modelRender->isMaterialOverriden()) {
        if (Visuals::removeHands(info.model->name) || Visuals::removeSleeves(info.model->name))
            return;
        static Chams chams;
        chams.render(ctx, state, info, customBoneToWorld);
        hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
        interfaces.modelRender->forceMaterialOverride(nullptr);
    }
    else
        hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
}

static bool __stdcall hookedSvCheatsGetBool()
{
    static auto _this = interfaces.cvar->findVar("sv_cheats");

    if (reinterpret_cast<std::uintptr_t>(_ReturnAddress()) == memory.cameraThink)
        return true;
    else
        return hooks.svCheats.getOriginal<bool(__thiscall*)(void*)>(13)(_this);
}

Hooks::Hooks()
{
    originalPresent = **reinterpret_cast<decltype(&originalPresent)*>(memory.present);
    **reinterpret_cast<void***>(memory.present) = reinterpret_cast<void*>(&hookedPresent);
    originalReset = **reinterpret_cast<decltype(&originalReset)*>(memory.reset);
    **reinterpret_cast<void***>(memory.reset) = reinterpret_cast<void*>(&hookedReset);

    clientMode.hookAt(24, hookedCreateMove);
    clientMode.hookAt(44, hookedDoPostScreenEffects);
    clientMode.hookAt(35, hookedGetViewModelFov);

    modelRender.hookAt(21, hookedDrawModelExecute);

    svCheats.hookAt(13, hookedSvCheatsGetBool);
}

Hooks::Vmt::Vmt(void* const base)
{
    oldVmt = *reinterpret_cast<std::uintptr_t**>(base);
    length = calculateLength(oldVmt) + 1;

    try {
        newVmt = findFreeDataPage(base, length);
    }
    catch (const std::runtime_error& e) {
        MessageBox(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
        std::exit(EXIT_FAILURE);
    }
    std::copy(oldVmt - 1, oldVmt - 1 + length, newVmt);
    *reinterpret_cast<std::uintptr_t**>(base) = newVmt + 1;
}

std::uintptr_t* Hooks::Vmt::findFreeDataPage(void* const base, std::size_t vmtSize)
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

    throw std::runtime_error{ "Could not find free memory pages in module at " + std::to_string(reinterpret_cast<int>(moduleInfo.lpBaseOfDll)) };
}

std::size_t Hooks::Vmt::calculateLength(std::uintptr_t* vmt) const noexcept
{
    std::size_t length{ 0 };
    MEMORY_BASIC_INFORMATION memoryInfo;
    while (VirtualQuery(reinterpret_cast<LPCVOID>(vmt[length]), &memoryInfo, sizeof(memoryInfo)) && memoryInfo.Protect == PAGE_EXECUTE_READ)
        length++;
    return length;
}
