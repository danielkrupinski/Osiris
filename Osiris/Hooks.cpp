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
#include "Hacks/Esp.h"
#include "Hacks/Backtrack.h"
#include "SDK/Panel.h"
#include "SDK/InputSystem.h"
#include "SDK/GameUI.h"
#include "SDK/Surface.h"
#include "Hacks/SkinChanger.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT __stdcall hookedWndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (msg == WM_KEYDOWN && LOWORD(wParam) == config.misc.menuKey) {
        gui.isOpen = !gui.isOpen;
        if (!gui.isOpen) {
            ImGui::GetIO().MouseDown[0] = false;
            interfaces.inputSystem->resetInputState();
        }
    }
    if (gui.isOpen && !ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
        return true;
    return CallWindowProc(hooks.originalWndProc, window, msg, wParam, lParam);
}

static HRESULT __stdcall hookedPresent(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
{
    static bool imguiInit{ ImGui_ImplDX9_Init(device) };

    if (gui.isOpen) {
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
        vertexDeclaration->Release();
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
    auto result = hooks.clientMode.callOriginal<bool, float, UserCmd*>(24, inputSampleTime, cmd);

    if (!cmd->command_number)
        return result;

    bool& sendPacket = *reinterpret_cast<bool*>(*(static_cast<uintptr_t*>(_AddressOfReturnAddress()) - 1) - 0x1C);

    memory.globalVars->serverTime(cmd);
    Misc::prepareRevolver(cmd);
    Misc::sniperCrosshair();
    Misc::recoilCrosshair();
    Visuals::skybox();
    Misc::bunnyHop(cmd);
    Misc::autoStrafe(cmd);
    Misc::removeCrouchCooldown(cmd);
    Aimbot::run(cmd);
    Triggerbot::run(cmd);
    Misc::autoPistol(cmd);
    Misc::autoReload(cmd);
    Misc::animateClanTag();
    Misc::revealRanks(cmd);
    Backtrack::run(cmd);
    cmd->viewangles.normalize();
    cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
    cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
    cmd->viewangles.z = 0.0f;

    if (!(cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2)))
        Misc::chokePackets(sendPacket);
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
    return hooks.clientMode.callOriginal<int, int>(44, param);
}

static float __stdcall hookedGetViewModelFov() noexcept
{
    return hooks.clientMode.callOriginal<float>(35) + static_cast<float>(config.visuals.viewmodelFov);
}

static void __stdcall hookedDrawModelExecute(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    if (interfaces.engine->isInGame() && !interfaces.modelRender->isMaterialOverriden()) {
        if (Visuals::removeHands(info.model->name) || Visuals::removeSleeves(info.model->name) || Visuals::removeWeapons(info.model->name))
            return;
        static Chams chams;
        chams.render(ctx, state, info, customBoneToWorld);
        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
        interfaces.modelRender->forceMaterialOverride(nullptr);
    } else
        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
}

static bool __fastcall hookedSvCheatsGetBool(void* _this) noexcept
{
    if (reinterpret_cast<uintptr_t>(_ReturnAddress()) == memory.cameraThink && config.visuals.thirdperson)
        return true;
    else
        return hooks.svCheats.callOriginal<bool>(13);
}

static void __stdcall hookedPaintTraverse(unsigned int panel, bool forceRepaint, bool allowForce) noexcept
{
    if (interfaces.panel->getName(panel) == "MatSystemTopPanel") {
        Misc::watermark();
        Misc::spectatorList();
        Esp::render();
    }
    hooks.panel.callOriginal<void, unsigned int, bool, bool>(41, panel, forceRepaint, allowForce);
}

static void __stdcall hookedFrameStageNotify(FrameStage stage) noexcept
{
    static auto backtrackInit = (Backtrack::init(), false);

    if (interfaces.engine->isInGame()) {
        Visuals::removeVisualRecoil(stage);
        Misc::fixAnimationLOD(stage);
        Backtrack::update(stage);
        SkinChanger::run(stage);
    }
    hooks.client.callOriginal<void, FrameStage>(37, stage);
}

struct SoundData {
    std::byte data[12];
    const char* soundEntry;
    std::byte pad[56];
};

static void __stdcall emitSound(SoundData data) noexcept
{
    if (config.misc.autoAccept && !strcmp(data.soundEntry, "UIPanorama.popup_accept_match_beep")) {
        memory.acceptMatch("");
        auto window = FindWindowA("Valve001", NULL);
        FLASHWINFO flash{ sizeof(FLASHWINFO), window, FLASHW_TRAY | FLASHW_TIMERNOFG, 0, 0 };
        FlashWindowEx(&flash);
        ShowWindow(window, SW_RESTORE);
    }
    hooks.sound.callOriginal<void, SoundData>(5, data);
}

static bool __stdcall shouldDrawFog() noexcept
{
    return !config.visuals.noFog;
}

static void __stdcall lockCursor() noexcept
{
    if (gui.isOpen)
        return interfaces.surface->unlockCursor();
    return hooks.surface.callOriginal<void>(67);
}

static void __stdcall setDrawColor(int r, int g, int b, int a) noexcept
{
    auto returnAddress = reinterpret_cast<uintptr_t>(_ReturnAddress());
    if (config.visuals.noScopeOverlay && (returnAddress == memory.scopeArc || returnAddress == memory.scopeLens)) {
        a = 0;
        *memory.disablePostProcessing = true;
    }
    hooks.surface.callOriginal<void, int, int, int, int>(15, r, g, b, a);
}

extern void initializeNSkinz();

Hooks::Hooks() noexcept
{
    initializeNSkinz();
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
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
    char buffer[MAX_PATH];
    if (GetWindowsDirectoryA(buffer, MAX_PATH))
        io.Fonts->AddFontFromFileTTF(strcat(buffer, "/Fonts/Tahoma.ttf"), 16.0f);

    originalWndProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtr(window, GWLP_WNDPROC, LONG_PTR(hookedWndProc))
        );

    originalPresent = **reinterpret_cast<decltype(originalPresent)**>(memory.present);
    **reinterpret_cast<void***>(memory.present) = reinterpret_cast<void*>(hookedPresent);
    originalReset = **reinterpret_cast<decltype(originalReset)**>(memory.reset);
    **reinterpret_cast<void***>(memory.reset) = reinterpret_cast<void*>(hookedReset);

    client.hookAt(37, hookedFrameStageNotify);
    clientMode.hookAt(17, shouldDrawFog);
    clientMode.hookAt(24, hookedCreateMove);
    clientMode.hookAt(44, hookedDoPostScreenEffects);
    clientMode.hookAt(35, hookedGetViewModelFov);
    modelRender.hookAt(21, hookedDrawModelExecute);
    panel.hookAt(41, hookedPaintTraverse);
    sound.hookAt(5, emitSound);
    surface.hookAt(15, setDrawColor);
    surface.hookAt(67, lockCursor);
    svCheats.hookAt(13, hookedSvCheatsGetBool);

    interfaces.gameUI->messageBox("This was a triumph!", "Osiris has been successfully loaded.");
}

void Hooks::restore() noexcept
{
    client.restore();
    clientMode.restore();
    modelRender.restore();
    panel.restore();
    sound.restore();
    surface.restore();
    svCheats.restore();

    netvars.restore();

    SetWindowLongPtr(FindWindowA("Valve001", NULL), GWLP_WNDPROC, LONG_PTR(originalWndProc));
    **reinterpret_cast<void***>(memory.present) = originalPresent;
    **reinterpret_cast<void***>(memory.reset) = originalReset;
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

auto Hooks::Vmt::calculateLength(uintptr_t* vmt) noexcept
{
    size_t length{ 0 };
    MEMORY_BASIC_INFORMATION memoryInfo;
    while (VirtualQuery(reinterpret_cast<LPCVOID>(vmt[length]), &memoryInfo, sizeof(memoryInfo)) && memoryInfo.Protect == PAGE_EXECUTE_READ)
        length++;
    return length;
}

Hooks::Vmt::Vmt(void* const base) noexcept
{
    this->base = base;
    oldVmt = *reinterpret_cast<uintptr_t**>(base);
    length = calculateLength(oldVmt) + 1;

    if (newVmt = findFreeDataPage(base, length)) {
        std::copy(oldVmt - 1, oldVmt - 1 + length, newVmt);
        *reinterpret_cast<uintptr_t**>(base) = newVmt + 1;
    }
}

void Hooks::Vmt::restore() noexcept
{
    *reinterpret_cast<uintptr_t**>(base) = oldVmt;
    ZeroMemory(newVmt, length * sizeof(uintptr_t));
}
