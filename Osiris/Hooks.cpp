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
#include "Hacks/AntiAim.h"
#include "Hacks/Backtrack.h"
#include "Hacks/Chams.h"
#include "Hacks/Esp.h"
#include "Hacks/Glow.h"
#include "Hacks/Misc.h"
#include "Hacks/Reportbot.h"
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
#include "SDK/MaterialSystem.h"
#include "SDK/ModelRender.h"
#include "SDK/Panel.h"
#include "SDK/RenderContext.h"
#include "SDK/ResourceAccessControl.h"
#include "SDK/SoundInfo.h"
#include "SDK/SoundEmitter.h"
#include "SDK/Surface.h"
#include "SDK/UserCmd.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT __stdcall wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (msg == WM_KEYDOWN && LOWORD(wParam) == config.misc.menuKey
        || ((msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) && config.misc.menuKey == VK_LBUTTON)
        || ((msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) && config.misc.menuKey == VK_RBUTTON)
        || ((msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) && config.misc.menuKey == VK_MBUTTON)
        || ((msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) && config.misc.menuKey == HIWORD(wParam) + 4)) {
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
    uintptr_t* framePointer;
    __asm mov framePointer, ebp;
    bool& sendPacket = *reinterpret_cast<bool*>(*framePointer - 0x1C);

    auto result = hooks.clientMode.callOriginal<bool, float, UserCmd*>(24, inputSampleTime, cmd);

    if (!cmd->commandNumber)
        return result;

    static auto previousViewAngles{ cmd->viewangles };
    const auto currentViewAngles{ cmd->viewangles };

    memory.globalVars->serverTime(cmd);
    Misc::nadePredict();
    Misc::antiAfkKick(cmd);
    Misc::fastPlant(cmd);
    Misc::prepareRevolver(cmd);
    Misc::sniperCrosshair();
    Misc::recoilCrosshair();
    Visuals::removeShadows();
    Visuals::skybox();
    Reportbot::run();
    Misc::bunnyHop(cmd);
    Misc::autoStrafe(cmd);
    Misc::removeCrouchCooldown(cmd);
    Aimbot::run(cmd);
    Triggerbot::run(cmd);
    Misc::autoPistol(cmd);
    Misc::autoReload(cmd);
    Misc::updateClanTag();
    Misc::fakeVote();
    Misc::fakeBan();
    Misc::stealNames();
    Misc::revealRanks(cmd);
    Backtrack::run(cmd);
    Misc::quickReload(cmd);
    Misc::moonwalk(cmd);
    Misc::quickHealthshot(cmd);
    Misc::fixTabletSignal();

    if (!(cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2))) {
        Misc::chokePackets(sendPacket);
        AntiAim::run(cmd, previousViewAngles, currentViewAngles, sendPacket);
    }

    auto viewAnglesDelta{ cmd->viewangles - previousViewAngles };
    viewAnglesDelta.normalize();
    viewAnglesDelta.x = std::clamp(viewAnglesDelta.x, -config.misc.maxAngleDelta, config.misc.maxAngleDelta);
    viewAnglesDelta.y = std::clamp(viewAnglesDelta.y, -config.misc.maxAngleDelta, config.misc.maxAngleDelta);

    cmd->viewangles = previousViewAngles + viewAnglesDelta;

    cmd->viewangles.normalize();
    Misc::fixMovement(cmd, currentViewAngles.y);

    cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
    cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
    cmd->viewangles.z = 0.0f;

    previousViewAngles = cmd->viewangles;

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
        if (const auto activeWeapon = localPlayer->getActiveWeapon(); activeWeapon && activeWeapon->getClientClass()->classId == ClassId::Tablet)
            additionalFov = 0.0f;
    }

    return hooks.clientMode.callOriginal<float>(35) + additionalFov;
}

static void __stdcall drawModelExecute(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    if (interfaces.engine->isInGame()) {
        if (Visuals::removeHands(info.model->name) || Visuals::removeSleeves(info.model->name) || Visuals::removeWeapons(info.model->name))
            return;
        const auto isOverridden = interfaces.modelRender->isMaterialOverridden();
        static Chams chams;
        if (chams.render(ctx, state, info, customBoneToWorld))
            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
        if (!isOverridden)
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
        Misc::drawBombTimer();
        Misc::watermark();
        Misc::spectatorList();
        Esp::render();
    }
    hooks.panel.callOriginal<void, unsigned int, bool, bool>(41, panel, forceRepaint, allowForce);
}

static void __stdcall frameStageNotify(FrameStage stage) noexcept
{
    static auto backtrackInit = (Backtrack::init(), false);

    if (interfaces.engine->isConnected() && !interfaces.engine->isInGame())
        Misc::changeName(true, nullptr, 0.0f);

    if (interfaces.engine->isInGame()) {
        Visuals::removeVisualRecoil(stage);
        Visuals::applyZoom(stage);
        Misc::fixAnimationLOD(stage);
        Backtrack::update(stage);
        SkinChanger::run(stage);
    }
    hooks.client.callOriginal<void, FrameStage>(37, stage);
}

struct SoundData {
    std::byte pad[4];
    int entityIndex;
    int channel;
    const char* soundEntry;
    std::byte pad1[8];
    float volume;
    std::byte pad2[44];
};

static void __stdcall emitSound(SoundData data) noexcept
{
    auto modulateVolume = [&data](int(*get)(int)) {
        if (auto entity{ interfaces.entityList->getEntity(data.entityIndex) }; entity && entity->isPlayer()) {
            if (data.entityIndex == interfaces.engine->getLocalPlayer())
                data.volume *= get(0) / 100.0f;
            else if (!entity->isEnemy())
                data.volume *= get(1) / 100.0f;
            else
                data.volume *= get(2) / 100.0f;
        }
    };

    modulateVolume([](int index) { return config.sound.players[index].masterVolume; });

    if (strstr(data.soundEntry, "Weapon") && strstr(data.soundEntry, "Single")) {
        modulateVolume([](int index) { return config.sound.players[index].weaponVolume; });
    } else if (config.misc.autoAccept && !strcmp(data.soundEntry, "UIPanorama.popup_accept_match_beep")) {
        memory.acceptMatch("");
        auto window = FindWindowW(L"Valve001", NULL);
        FLASHWINFO flash{ sizeof(FLASHWINFO), window, FLASHW_TRAY | FLASHW_TIMERNOFG, 0, 0 };
        FlashWindowEx(&flash);
        ShowWindow(window, SW_RESTORE);
    }
    data.volume = std::clamp(data.volume, 0.0f, 1.0f);
    hooks.sound.callOriginal<void, SoundData>(5, data);
}

static bool __stdcall shouldDrawFog() noexcept
{
    return !config.visuals.noFog;
}

static bool __stdcall shouldDrawViewModel() noexcept
{
    if (auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()); config.visuals.zoom && localPlayer && localPlayer->fov() < 45 && localPlayer->fovStart() < 45)
        return false;
    return hooks.clientMode.callOriginal<bool>(27);
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
    switch (fnv::hashRuntime(event->getName())) {
    case fnv::hash("player_death"):
        Misc::killMessage(event);
        SkinChanger::overrideHudIcon(event);
        break;
    case fnv::hash("player_hurt"):
        Misc::playHitSound(event);
        Visuals::hitMarker(event);
        break;
    }
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
        && !interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->isScoped())
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
        auto modulateVolume = [&soundInfo](int(*get)(int)) {
            if (auto entity{ interfaces.entityList->getEntity(soundInfo.entityIndex) }; entity && entity->isPlayer()) {
                if (soundInfo.entityIndex == interfaces.engine->getLocalPlayer())
                    soundInfo.volume *= get(0) / 100.0f;
                else if (!entity->isEnemy())
                    soundInfo.volume *= get(1) / 100.0f;
                else
                    soundInfo.volume *= get(2) / 100.0f;
            }
        };

        modulateVolume([](int index) { return config.sound.players[index].masterVolume; });

        if (!strcmp(soundName, "Player.DamageHelmetFeedback"))
            modulateVolume([](int index) { return config.sound.players[index].headshotVolume; });
        else if (strstr(soundName, "Step"))
            modulateVolume([](int index) { return config.sound.players[index].footstepVolume; });
        else if (strstr(soundName, "Chicken"))
            soundInfo.volume *= config.sound.chickenVolume / 100.0f;
    }
    soundInfo.volume = std::clamp(soundInfo.volume, 0.0f, 1.0f);
    return hooks.originalDispatchSound(soundInfo);
}

static int __stdcall render2dEffectsPreHud(int param) noexcept
{
    Visuals::applyScreenEffects();
    Visuals::hitMarker();
    return hooks.viewRender.callOriginal<int, int>(39, param);
}

static void* __stdcall getDemoPlaybackParameters() noexcept
{
    auto result = hooks.engine.callOriginal<void*>(218);

    constexpr bool overwatchRevealTest = true;
    if constexpr (overwatchRevealTest) {
        if (uintptr_t returnAddress = uintptr_t(_ReturnAddress()); returnAddress == memory.test || returnAddress == memory.test2)
            return nullptr;
    }
    return result;
}

static bool __stdcall isPlayingDemo() noexcept
{
    if (config.misc.revealMoney
        && *static_cast<uintptr_t*>(_ReturnAddress()) == 0x0975C084  // client_panorama.dll : 84 C0 75 09 38 05
        && **reinterpret_cast<uintptr_t**>(uintptr_t(_AddressOfReturnAddress()) + 4) == 0x0C75C084) { // client_panorama.dll : 84 C0 75 0C 5B
        return true;
    }
    return hooks.engine.callOriginal<bool>(82);
}

Hooks::Hooks() noexcept
{
    SkinChanger::initializeKits();
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

    originalWndProc = WNDPROC(SetWindowLongPtrA(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(wndProc)));

    originalPresent = **reinterpret_cast<decltype(originalPresent)**>(memory.present);
    **reinterpret_cast<decltype(present)***>(memory.present) = present;
    originalReset = **reinterpret_cast<decltype(originalReset)**>(memory.reset);
    **reinterpret_cast<decltype(reset)***>(memory.reset) = reset;

    bspQuery.hookAt(6, listLeavesInBox);
    client.hookAt(37, frameStageNotify);
    clientMode.hookAt(17, shouldDrawFog);
    clientMode.hookAt(18, overrideView);
    clientMode.hookAt(24, createMove);
    clientMode.hookAt(27, shouldDrawViewModel);
    clientMode.hookAt(35, getViewModelFov);
    clientMode.hookAt(44, doPostScreenEffects);
    engine.hookAt(82, isPlayingDemo);
    engine.hookAt(218, getDemoPlaybackParameters);
    gameEventManager.hookAt(9, fireEventClientSide);
    modelRender.hookAt(21, drawModelExecute);
    panel.hookAt(41, paintTraverse);
    sound.hookAt(5, emitSound);
    surface.hookAt(15, setDrawColor);
    surface.hookAt(67, lockCursor);
    svCheats.hookAt(13, svCheatsGetBool);
    viewRender.hookAt(39, render2dEffectsPreHud);

    if (DWORD oldProtection; VirtualProtect(memory.dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        originalDispatchSound = decltype(originalDispatchSound)(uintptr_t(memory.dispatchSound + 1) + *memory.dispatchSound);
        *memory.dispatchSound = uintptr_t(dispatchSound) - uintptr_t(memory.dispatchSound + 1);
        VirtualProtect(memory.dispatchSound, 4, oldProtection, nullptr);
    }

    interfaces.gameUI->messageBox("This was a triumph!", "Osiris has been successfully loaded.");
}

void Hooks::restore() noexcept
{
    bspQuery.restore();
    client.restore();
    clientMode.restore();
    engine.restore();
    gameEventManager.restore();
    modelRender.restore();
    panel.restore();
    sound.restore();
    surface.restore();
    svCheats.restore();
    viewRender.restore();

    netvars.restore();

    Glow::clearCustomObjects();

    SetWindowLongPtrA(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(originalWndProc));
    **reinterpret_cast<void***>(memory.present) = originalPresent;
    **reinterpret_cast<void***>(memory.reset) = originalReset;

    if (DWORD oldProtection; VirtualProtect(memory.dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        *memory.dispatchSound = uintptr_t(originalDispatchSound) - uintptr_t(memory.dispatchSound + 1);
        VirtualProtect(memory.dispatchSound, 4, oldProtection, nullptr);
    }

    interfaces.resourceAccessControl->accessingThreadCount--;
}

uintptr_t* Hooks::Vmt::findFreeDataPage(void* const base, size_t vmtSize) noexcept
{
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(base, &mbi, sizeof(mbi));
    MODULEINFO moduleInfo;
    GetModuleInformation(GetCurrentProcess(), static_cast<HMODULE>(mbi.AllocationBase), &moduleInfo, sizeof(moduleInfo));

    auto moduleEnd{ reinterpret_cast<uintptr_t*>(static_cast<std::byte*>(moduleInfo.lpBaseOfDll) + moduleInfo.SizeOfImage) };

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
    while (VirtualQuery(LPCVOID(vmt[length]), &memoryInfo, sizeof(memoryInfo)) && memoryInfo.Protect == PAGE_EXECUTE_READ)
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
