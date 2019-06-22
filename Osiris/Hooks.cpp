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
#include "Hooks.h"
#include "Interfaces.h"
#include "Memory.h"

#include "Hacks/Aimbot.h"
#include "Hacks/Backtrack.h"
#include "Hacks/Chams.h"
#include "Hacks/Esp.h"
#include "Hacks/Glow.h"
#include "Hacks/Misc.h"
#include "Hacks/SkinChanger.h"
#include "Hacks/Triggerbot.h"
#include "Hacks/Visuals.h"

#include "SDK/Engine.h"
#include "SDK/Entity.h"
#include "SDK/EntityList.h"
#include "SDK/GameEvent.h"
#include "SDK/GameEventManager.h"
#include "SDK/GameUI.h"
#include "SDK/InputSystem.h"
#include "SDK/ModelRender.h"
#include "SDK/Panel.h"
#include "SDK/SoundInfo.h"
#include "SDK/SoundEmitter.h"
#include "SDK/Surface.h"
#include "SDK/UserCmd.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT __stdcall wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
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

static HRESULT __stdcall present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
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

static HRESULT __stdcall reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    auto result = hooks.originalReset(device, params);
    ImGui_ImplDX9_CreateDeviceObjects();
    return result;
}

static bool __stdcall createMove(float inputSampleTime, UserCmd* cmd) noexcept
{
    auto result = hooks.clientMode.callOriginal<bool, float, UserCmd*>(24, inputSampleTime, cmd);

    if (!cmd->command_number)
        return result;

    bool& sendPacket = *reinterpret_cast<bool*>(*(static_cast<uintptr_t*>(_AddressOfReturnAddress()) - 1) - 0x1C);

    memory.globalVars->serverTime(cmd);
    Misc::fastPlant(cmd);
    Misc::prepareRevolver(cmd);
    Misc::sniperCrosshair();
    Misc::recoilCrosshair();
    Visuals::removeShadows();
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

static int __stdcall doPostScreenEffects(int param) noexcept
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

static float __stdcall getViewModelFov() noexcept
{
    float additionalFov = static_cast<float>(config.visuals.viewmodelFov);
    if (const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())) {
        if (const auto activeWeapon = interfaces.entityList->getEntityFromHandle(localPlayer->getProperty<int>("m_hActiveWeapon")); activeWeapon && activeWeapon->getClientClass()->classId == ClassId::Tablet)
            additionalFov = 0.0f;
    }

    return hooks.clientMode.callOriginal<float>(35) + additionalFov;
}

static void __stdcall drawModelExecute(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
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

static bool __stdcall svCheatsGetBool() noexcept
{
    if (reinterpret_cast<uintptr_t>(_ReturnAddress()) == memory.cameraThink && config.visuals.thirdperson)
        return true;
    else
        return hooks.svCheats.callOriginal<bool>(13);
}

static void __stdcall paintTraverse(unsigned int panel, bool forceRepaint, bool allowForce) noexcept
{
    if (interfaces.panel->getName(panel) == "MatSystemTopPanel") {
        Misc::watermark();
        Misc::spectatorList();
        Esp::render();
    }
    hooks.panel.callOriginal<void, unsigned int, bool, bool>(41, panel, forceRepaint, allowForce);
}

static void __stdcall frameStageNotify(FrameStage stage) noexcept
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
    std::byte pad[12];
    const char* soundEntry;
    std::byte pad1[8];
    float volume;
    std::byte pad2[44];
};

static void __stdcall emitSound(SoundData data) noexcept
{
    if (strstr(data.soundEntry, "Weapon") && strstr(data.soundEntry, "Single"))
       data.volume *= config.misc.weaponSoundsVolume / 100.0f;
    else if (config.misc.autoAccept && !strcmp(data.soundEntry, "UIPanorama.popup_accept_match_beep")) {
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

static bool __stdcall fireEventClientSide(GameEvent* event) noexcept
{
    Misc::killMessage(event);
    Misc::playHitSound(event);
    SkinChanger::overrideHudIcon(event);
    return hooks.gameEventManager.callOriginal<bool, GameEvent*>(9, event);
}

struct ViewSetup {
    std::byte pad[176];
    float fov;
    std::byte pad1[32];
    float farZ;
};

static void __stdcall overrideView(ViewSetup* setup) noexcept
{
    if (interfaces.engine->isInGame()
        && !interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<bool>("m_bIsScoped"))
        setup->fov += config.visuals.fov;
    setup->farZ += config.visuals.farZ * 10;
    hooks.clientMode.callOriginal<void, ViewSetup*>(18, setup);
}

struct RenderableInfo {
    Entity* renderable;
    std::byte pad[18];
    uint16_t flags;
    uint16_t flags2;
};

static int __stdcall listLeavesInBox(const Vector& mins, const Vector& maxs, unsigned short* list, int listMax) noexcept
{
    if (config.misc.disableModelOcclusion && reinterpret_cast<uintptr_t>(_ReturnAddress()) == memory.listLeaves) {
        if (auto info = *reinterpret_cast<RenderableInfo**>(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + 0x14); info && info->renderable) {
            if (auto ent = callVirtualMethod<Entity*>(info->renderable - 4, 7); ent && ent->isPlayer()) {
                info->flags &= ~0x100;
                info->flags2 |= 0xC0;

                constexpr float maxCoord{ 16384.0f };
                constexpr float minCoord{ -maxCoord };
                constexpr Vector min{ minCoord, minCoord, minCoord };
                constexpr Vector max{ maxCoord, maxCoord, maxCoord };
                return hooks.bspQuery.callOriginal<int, const Vector&, const Vector&, unsigned short*, int>(6, min, max, list, listMax);
            }
        }
    }
    return hooks.bspQuery.callOriginal<int, const Vector&, const Vector&, unsigned short*, int>(6, mins, maxs, list, listMax);
}

static int __fastcall dispatchSound(SoundInfo& soundInfo) noexcept
{
    if (const char* soundName = interfaces.soundEmitter->getSoundName(soundInfo.soundIndex)) {
        if (!strcmp(soundName, "Player.DamageHelmetFeedback"))
            soundInfo.volume *= config.misc.headshotSoundVolume / 100.0f;
    }
    return hooks.originalDispatchSound(soundInfo);
}

Hooks::Hooks() noexcept
{
    SkinChanger::initializeKits();
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

    static ImWchar ranges[] = { 0x0020, 0x00FF, 0x0100, 0x017f, 0 };
    char buffer[MAX_PATH];
    if (GetWindowsDirectoryA(buffer, MAX_PATH))
        io.Fonts->AddFontFromFileTTF(strcat(buffer, "/Fonts/Tahoma.ttf"), 16.0f, nullptr, ranges);

    originalWndProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtr(window, GWLP_WNDPROC, LONG_PTR(wndProc))
        );

    originalPresent = **reinterpret_cast<decltype(originalPresent)**>(memory.present);
    **reinterpret_cast<void***>(memory.present) = reinterpret_cast<void*>(present);
    originalReset = **reinterpret_cast<decltype(originalReset)**>(memory.reset);
    **reinterpret_cast<void***>(memory.reset) = reinterpret_cast<void*>(reset);

    bspQuery.hookAt(6, listLeavesInBox);
    client.hookAt(37, frameStageNotify);
    clientMode.hookAt(17, shouldDrawFog);
    clientMode.hookAt(18, overrideView);
    clientMode.hookAt(24, createMove);
    clientMode.hookAt(44, doPostScreenEffects);
    clientMode.hookAt(35, getViewModelFov);
    gameEventManager.hookAt(9, fireEventClientSide);
    modelRender.hookAt(21, drawModelExecute);
    panel.hookAt(41, paintTraverse);
    sound.hookAt(5, emitSound);
    surface.hookAt(15, setDrawColor);
    surface.hookAt(67, lockCursor);
    svCheats.hookAt(13, svCheatsGetBool);

    DWORD oldProtection;
    VirtualProtect(memory.dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection);
    originalDispatchSound = reinterpret_cast<decltype(originalDispatchSound)>(reinterpret_cast<uintptr_t>(memory.dispatchSound + 1) + *memory.dispatchSound);
    *memory.dispatchSound = reinterpret_cast<uintptr_t>(&dispatchSound) - reinterpret_cast<uintptr_t>(memory.dispatchSound + 1);
    VirtualProtect(memory.dispatchSound, 4, oldProtection, NULL);

    interfaces.gameUI->messageBox("This was a triumph!", "Osiris has been successfully loaded.");
}

void Hooks::restore() noexcept
{
    bspQuery.restore();
    client.restore();
    clientMode.restore();
    gameEventManager.restore();
    modelRender.restore();
    panel.restore();
    sound.restore();
    surface.restore();
    svCheats.restore();

    netvars.restore();

    SetWindowLongPtr(FindWindowA("Valve001", NULL), GWLP_WNDPROC, LONG_PTR(originalWndProc));
    **reinterpret_cast<void***>(memory.present) = originalPresent;
    **reinterpret_cast<void***>(memory.reset) = originalReset;

    DWORD oldProtection;
    VirtualProtect(memory.dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection);
    *memory.dispatchSound = reinterpret_cast<uintptr_t>(originalDispatchSound) - reinterpret_cast<uintptr_t>(memory.dispatchSound + 1);
    VirtualProtect(memory.dispatchSound, 4, oldProtection, NULL);
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
