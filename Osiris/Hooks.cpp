#define _CRT_SECURE_NO_WARNINGS
#include <intrin.h>
#include <string>
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
    static bool imguiInit{ ImGui_ImplDX9_Init(device) };

    static bool isMenuToggled{ false };

    if (gui.isOpen) {
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
    else if (isMenuToggled) {
            memory.input->isMouseInitialized = true;
            interfaces.inputSystem->enableInput(true);
            interfaces.inputSystem->resetInputState();
            isMenuToggled = false;
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
    auto result = hooks.clientMode.getOriginal<bool(__thiscall*)(ClientMode*, float, UserCmd*)>(24)(memory.clientMode, inputSampleTime, cmd);

    if (!cmd->command_number)
        return result;

    Misc::sniperCrosshair();
    Misc::recoilCrosshair();
    Visuals::skybox();
    Misc::bunnyHop(cmd);
    Misc::autoStrafe(cmd);
    Misc::removeCrouchCooldown(cmd);
    Aimbot::run(cmd);
    Triggerbot::run(cmd);
    Misc::autoPistol(cmd);
    Misc::animateClanTag();
    cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
    cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
    cmd->viewangles.z = 0.0f;
    return false;
}

static int __stdcall hookedDoPostScreenEffects(int param) noexcept
{
    if (interfaces.engine->isInGame()) {
        Visuals::modifySmoke();
        Visuals::thirdperson();
        Misc::inverseRagdollGravity();
        Visuals::disablePostProcessing();
        Visuals::colorWorld();
        Visuals::reduceFlashEffect();
        Visuals::removeBlur();
        Visuals::updateBrightness();
        Visuals::removeGrass();
        Visuals::remove3dSky();
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
        if (Visuals::removeHands(info.model->name) || Visuals::removeSleeves(info.model->name) || Visuals::removeWeapons(info.model->name))
            return;
        static Chams chams;
        chams.render(ctx, state, info, customBoneToWorld);
        hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
        interfaces.modelRender->forceMaterialOverride(nullptr);
    }
    else
        hooks.modelRender.getOriginal<void(__thiscall*)(ModelRender*, void*, void*, const ModelRenderInfo&, matrix3x4*)>(21)(interfaces.modelRender, ctx, state, info, customBoneToWorld);
}

static bool __stdcall hookedSvCheatsGetBool() noexcept
{
    static auto _this = interfaces.cvar->findVar("sv_cheats");

    if (reinterpret_cast<uintptr_t>(_ReturnAddress()) == memory.cameraThink && config.visuals.thirdperson)
        return true;
    else
        return hooks.svCheats.getOriginal<bool(__thiscall*)(void*)>(13)(_this);
}

static void __stdcall hookedPaintTraverse(unsigned int panel, bool forceRepaint, bool allowForce) noexcept
{
    if (!config.visuals.noScopeOverlay || interfaces.panel->getName(panel) != "HudZoom")
        hooks.panel.getOriginal<void(__thiscall*)(Panel*, unsigned int, bool, bool)>(41)(interfaces.panel, panel, forceRepaint, allowForce);

    if (interfaces.panel->getName(panel) == "MatSystemTopPanel") {
        Misc::watermark();
        Misc::spectatorList();
    }
}

static void __stdcall hookedFrameStageNotify(FrameStage stage) noexcept
{
    if (interfaces.engine->isInGame()) {
        Visuals::removeVisualRecoil(stage);
    }

    hooks.client.getOriginal<void(__thiscall*)(Client*, FrameStage)>(37)(interfaces.client, stage);
}

struct SoundData {
    std::byte data[12];
    const char* soundEntry;
    std::byte pad[56];
};

static void __stdcall hookedEmitSound(SoundData data) noexcept
{
    if (config.misc.autoAccept && !strcmp(data.soundEntry, "UIPanorama.popup_accept_match_beep")) {
        memory.acceptMatch("");
        auto window = FindWindowA("Valve001", NULL);
        FLASHWINFO flash{ sizeof(FLASHWINFO), window, FLASHW_TRAY | FLASHW_TIMERNOFG, 0, 0 };
        FlashWindowEx(&flash);
        ShowWindow(window, SW_RESTORE);
    }
    hooks.sound.getOriginal<void(__thiscall*)(Sound*, SoundData)>(5)(interfaces.sound, data);
}

Hooks::Hooks() noexcept
{
    ImGui::CreateContext();
    auto window = FindWindowA("Valve001", NULL);
    ImGui_ImplWin32_Init(window);

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
    if (GetWindowsDirectoryA(buffer, MAX_PATH))
        io.Fonts->AddFontFromFileTTF(strcat(buffer, "/Fonts/Tahoma.ttf"), 16.0f);

    originalWndProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtr(window, GWLP_WNDPROC, LONG_PTR(hookedWndProc))
        );

    originalPresent = **reinterpret_cast<decltype(&originalPresent)*>(memory.present);
    **reinterpret_cast<void***>(memory.present) = reinterpret_cast<void*>(&hookedPresent);
    originalReset = **reinterpret_cast<decltype(&originalReset)*>(memory.reset);
    **reinterpret_cast<void***>(memory.reset) = reinterpret_cast<void*>(&hookedReset);

    client.hookAt(37, hookedFrameStageNotify);

    clientMode.hookAt(24, hookedCreateMove);
    clientMode.hookAt(44, hookedDoPostScreenEffects);
    clientMode.hookAt(35, hookedGetViewModelFov);

    modelRender.hookAt(21, hookedDrawModelExecute);

    panel.hookAt(41, hookedPaintTraverse);

    sound.hookAt(5, hookedEmitSound);

    svCheats.hookAt(13, hookedSvCheatsGetBool);

    interfaces.gameUI->messageBox("This was a triumph!", "Osiris has been successfully loaded.");
}

uintptr_t* Hooks::Vmt::findFreeDataPage(void* const base, size_t vmtSize) noexcept
{
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(base, &mbi, sizeof(mbi));
    MODULEINFO moduleInfo;
    GetModuleInformation(GetCurrentProcess(), static_cast<HMODULE>(mbi.AllocationBase), &moduleInfo, sizeof(moduleInfo));

    uintptr_t* moduleEnd{ reinterpret_cast<uintptr_t*>(static_cast<std::byte*>(moduleInfo.lpBaseOfDll) + moduleInfo.SizeOfImage) };

    for (auto currentAddress = moduleEnd - vmtSize; currentAddress > moduleInfo.lpBaseOfDll; currentAddress -= *currentAddress ? vmtSize : 1)
        if (!*currentAddress)
            if (VirtualQuery(currentAddress, &mbi, sizeof(mbi)) && mbi.State == MEM_COMMIT
                && mbi.Protect == PAGE_READWRITE && mbi.RegionSize >= vmtSize * sizeof(uintptr_t)
                && std::all_of(currentAddress, currentAddress + vmtSize, [](uintptr_t a) { return !a; }))
                return currentAddress;

    return nullptr;
}

auto Hooks::Vmt::calculateLength(uintptr_t * vmt) noexcept
{
    size_t length{ 0 };
    MEMORY_BASIC_INFORMATION memoryInfo;
    while (VirtualQuery(reinterpret_cast<LPCVOID>(vmt[length]), &memoryInfo, sizeof(memoryInfo)) && memoryInfo.Protect == PAGE_EXECUTE_READ)
        length++;
    return length;
}

Hooks::Vmt::Vmt(void* const base) noexcept
{
    oldVmt = *reinterpret_cast<uintptr_t**>(base);
    length = calculateLength(oldVmt) + 1;

    if (newVmt = findFreeDataPage(base, length)) {
        std::copy(oldVmt - 1, oldVmt - 1 + length, newVmt);
        *reinterpret_cast<uintptr_t**>(base) = newVmt + 1;
    }
}
