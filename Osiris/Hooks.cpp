#include <functional>
#include <intrin.h>
#include <string>
#include <Windows.h>
#include <Psapi.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "MinHook/MinHook.h"

#include "Config.h"
#include "EventListener.h"
#include "GameData.h"
#include "GUI.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "Memory.h"

#include "Hacks/Aimbot.h"
#include "Hacks/Ragebot.h"
#include "Hacks/AntiAim.h"
#include "Hacks/Backtrack.h"
#include "Hacks/Chams.h"
#include "Hacks/EnginePrediction.h"
#include "Hacks/StreamProofESP.h"
#include "Hacks/Glow.h"
#include "Hacks/Misc.h"
#include "Hacks/SkinChanger.h"
#include "Hacks/Triggerbot.h"
#include "Hacks/Visuals.h"

#include "SDK/Engine.h"
#include "SDK/Entity.h"
#include "SDK/EntityList.h"
#include "SDK/FrameStage.h"
#include "SDK/GameEvent.h"
#include "SDK/GameUI.h"
#include "SDK/InputSystem.h"
#include "SDK/MaterialSystem.h"
#include "SDK/ModelRender.h"
#include "SDK/Panel.h"
#include "SDK/RenderContext.h"
#include "SDK/SoundInfo.h"
#include "SDK/SoundEmitter.h"
#include "SDK/StudioRender.h"
#include "SDK/Surface.h"
#include "SDK/UserCmd.h"
#include "SDK/Beams.h"
#include "extraHooks.h"
#include "Hacks/Animations.h"
#include "SDK/Tickbase.h"
#include "Memory.h"
#include "Interfaces.h"
#include "SDK/Input.h"
#include "SDK/InputSystem.h"
#include "SDK/StudioRender.h"

int rageBestDmg = 0;
int rageBestChance = 0;
Vector quickPeekVector = {0, 0, 0};
Vector wallbangVector = {0, 0, 0};

static LRESULT __stdcall wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    static const auto once = [](HWND window) noexcept {
        netvars = std::make_unique<Netvars>();
        eventListener = std::make_unique<EventListener>();
        config = std::make_unique<Config>("Osiris");

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(window);
        gui = std::make_unique<GUI>();

        hooks->install();

        return true;
    }(window);

    if (msg == WM_KEYDOWN && LOWORD(wParam) == config->misc.menuKey
        || ((msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) && config->misc.menuKey == VK_LBUTTON)
        || ((msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) && config->misc.menuKey == VK_RBUTTON)
        || ((msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) && config->misc.menuKey == VK_MBUTTON)
        || ((msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) && config->misc.menuKey == HIWORD(wParam) + 4)) {
        gui->open = !gui->open;
        if (!gui->open) {
            // ImGui::GetIO().MouseDown[0] = false;
            interfaces->inputSystem->resetInputState();
        }
    }

    LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam);

    interfaces->inputSystem->enableInput(!gui->open);

    return CallWindowProcW(hooks->originalWndProc, window, msg, wParam, lParam);
}

static HRESULT __stdcall present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
{
    static bool imguiInit{ ImGui_ImplDX9_Init(device) };

    if (config->loadScheduledFonts())
        ImGui_ImplDX9_InvalidateDeviceObjects();

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    StreamProofESP::render();
    Misc::purchaseList();
    Misc::spectatorList();
    Misc::StatusBar();
	Misc::ShotsCout(nullptr, rageBestDmg, rageBestChance, quickPeekVector);
    Misc::DrawInaccuracy(ImGui::GetBackgroundDrawList());
    Misc::noscopeCrosshair(ImGui::GetBackgroundDrawList());
    Misc::recoilCrosshair(ImGui::GetBackgroundDrawList());
	Misc::drawStartPos(ImGui::GetBackgroundDrawList(), quickPeekVector);
    Misc::drawWallbangVector(ImGui::GetBackgroundDrawList(), wallbangVector, rageBestDmg, rageBestChance);
	Visuals::updateBeams();
	
    if (gui->open)
        gui->render();

    ImGui::EndFrame();
    ImGui::Render();

    if (device->BeginScene() == D3D_OK) {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    return hooks->originalPresent(device, src, dest, windowOverride, dirtyRegion);
}

static HRESULT __stdcall reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    return hooks->originalReset(device, params);
}


static int __fastcall SendDatagram(NetworkChannel* network, void* edx, void* datagram)
{
    auto original = hooks->networkChannel.getOriginal<int, 46, void*>( datagram);
    if (!config->backtrack.fakeLatency || datagram || !interfaces->engine->isInGame() || !config->backtrack.enabled)
    {
        return original(network, datagram);
    }
    int instate = network->InReliableState;
    int insequencenr = network->InSequenceNr;
    int faketimeLimit = config->backtrack.timeLimit; if (faketimeLimit <= 200) { faketimeLimit = 0; } else { faketimeLimit -= 200; }
    float delta = max(0.f, std::clamp(faketimeLimit / 1000.f, 0.f, Backtrack::cvars.maxUnlag->getFloat()) - network->getLatency(0));

    Backtrack::AddLatencyToNetwork(network, delta);

    int result = original(network, datagram);

    network->InReliableState = instate;
    network->InSequenceNr = insequencenr;

    return result;
}

static bool __stdcall createMove(float inputSampleTime, UserCmd* cmd) noexcept
{
    auto result = hooks->clientMode.callOriginal<bool, 24>(inputSampleTime, cmd);

    if (!cmd->commandNumber)
        return result;

    uintptr_t* framePointer;
    __asm mov framePointer, ebp;
    bool& sendPacket = *reinterpret_cast<bool*>(*framePointer - 0x1C);

    static auto previousViewAngles{ cmd->viewangles };
    const auto currentViewAngles{ cmd->viewangles };

    memory->globalVars->serverTime(cmd);
    Misc::nadePredict();
    Misc::antiAfkKick(cmd);
    Misc::prepareRevolver(cmd);
    Visuals::removeShadows();
    Misc::runReportbot();
    Misc::bunnyHop(cmd);
    Misc::autoStrafe(cmd);
    Misc::removeCrouchCooldown(cmd);
    Misc::autoPistol(cmd);
    Misc::autoReload(cmd);
    Misc::updateClanTag();
    Misc::fakeBan();
    Misc::stealNames();
    Misc::revealRanks(cmd);
    Misc::quickReload(cmd);
    Misc::quickHealthshot(cmd);
    Misc::fixTabletSignal();
    Misc::slowwalk(cmd);
    extraHook.init();

    static void* oldPointer = nullptr;

    auto network = interfaces->engine->getNetworkChannel();
    if (oldPointer != network && network && localPlayer)
    {
        oldPointer = network;
        Backtrack::UpdateIncomingSequences(true);
        hooks->networkChannel.init(network);
        hooks->networkChannel.hookAt(46, SendDatagram);
    }
    Backtrack::UpdateIncomingSequences();
	

    EnginePrediction::run(cmd);

    rageBestDmg = 0;
	rageBestChance = 0;
	wallbangVector = {0, 0,0};
	
    Aimbot::run(cmd);
    Ragebot::run(cmd, rageBestDmg, rageBestChance, wallbangVector);
    Tickbase::run(cmd, sendPacket);//run this after ragebot
    Triggerbot::run(cmd);
    Backtrack::run(cmd);
    Misc::edgejump(cmd);
    Misc::moonwalk(cmd);
    Misc::fastPlant(cmd);
	Misc::quickpeek(cmd, quickPeekVector);
	
    config->globals.serverTime = memory->globalVars->serverTime();
    config->globals.chokedPackets = interfaces->engine->getNetworkChannel()->chokedPackets;
    config->globals.tickRate = memory->globalVars->intervalPerTick;

    if (!(cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2)) || config->misc.fakeLagSelectedFlags[0])
	   if (config->misc.fakeLagKey == 0 || GetAsyncKeyState(config->misc.fakeLagKey))
			Misc::chokePackets(sendPacket, cmd);
     

	AntiAim::fakeWalk(cmd, sendPacket);
    Misc::fakeDuck(cmd, sendPacket);
    AntiAim::run(cmd, previousViewAngles, currentViewAngles, sendPacket);

    auto viewAnglesDelta{ cmd->viewangles - previousViewAngles };
    viewAnglesDelta.normalize();
    viewAnglesDelta.x = std::clamp(viewAnglesDelta.x, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);
    viewAnglesDelta.y = std::clamp(viewAnglesDelta.y, -config->misc.maxAngleDelta, config->misc.maxAngleDelta);

    cmd->viewangles = previousViewAngles + viewAnglesDelta;

    cmd->viewangles.normalize();
    Misc::fixMovement(cmd, currentViewAngles.y);

    cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
	    if (cmd->viewangles.y > 180.0f)
        cmd->viewangles.y = 180.0f - cmd->viewangles.y;
    else if (cmd->viewangles.y < -180.0f)
        cmd->viewangles.y = -180.0f + cmd->viewangles.y;

	
    cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
    cmd->viewangles.z = 0.0f;
    cmd->forwardmove = std::clamp(cmd->forwardmove, -450.0f, 450.0f);
    cmd->sidemove = std::clamp(cmd->sidemove, -450.0f, 450.0f);

    previousViewAngles = cmd->viewangles;


	if (sendPacket)
    {
        config->globals.fakeAngle = cmd->viewangles;
        config->globals.cmdAngle = cmd->viewangles;
        config->globals.thirdPersonAnglesSet = true;
    }
    else
    {
        config->globals.realAngle = cmd->viewangles;
        config->globals.cmdAngle = cmd->viewangles;
        config->globals.thirdPersonAnglesSet = true;
    }

	  config->globals.sendPacket = sendPacket;

    Animations::update(cmd, sendPacket);
    Animations::fake();

    return false;
}

static int __stdcall doPostScreenEffects(int param) noexcept
{
    if (interfaces->engine->isInGame()) {
        Visuals::thirdperson();
        Misc::inverseRagdollGravity();
        Visuals::reduceFlashEffect();
        Visuals::updateBrightness();
        Visuals::remove3dSky();
    	Visuals::thirdperson();
        Glow::render();
    }
    return hooks->clientMode.callOriginal<int, 44>(param);
}

static float __stdcall getViewModelFov() noexcept
{
    float additionalFov = static_cast<float>(config->visuals.viewmodelFov);
    if (localPlayer) {
        if (const auto activeWeapon = localPlayer->getActiveWeapon(); activeWeapon && activeWeapon->getClientClass()->classId == ClassId::Tablet)
            additionalFov = 0.0f;
    }

    return hooks->clientMode.callOriginal<float, 35>() + additionalFov;
}

static void __stdcall drawModelExecute(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    if (interfaces->studioRender->isForcedMaterialOverride())
        return hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);

    if (Visuals::removeHands(info.model->name) || Visuals::removeSleeves(info.model->name) || Visuals::removeWeapons(info.model->name))
        return;

    static Chams chams;
    if (!chams.render(ctx, state, info, customBoneToWorld))
        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
    interfaces->studioRender->forcedMaterialOverride(nullptr);
}

static bool __fastcall svCheatsGetBool(void* _this) noexcept
{
    if (uintptr_t(_ReturnAddress()) == memory->cameraThink && config->visuals.thirdperson)
        return true;
    else
        return hooks->svCheats.getOriginal<bool, 13>()(_this);
}

static void __stdcall paintTraverse(unsigned int panel, bool forceRepaint, bool allowForce) noexcept
{
    if (interfaces->panel->getName(panel) == "MatSystemTopPanel") {
        Misc::drawBombTimer();
        Misc::watermark();
        Visuals::hitMarker();
    	Visuals::indicators();
    }
    hooks->panel.callOriginal<void, 41>(panel, forceRepaint, allowForce);
}

static void __stdcall frameStageNotify(FrameStage stage) noexcept
{
    static auto backtrackInit = (Backtrack::init(), false);

    if (interfaces->engine->isConnected() && !interfaces->engine->isInGame())
        Misc::changeName(true, nullptr, 0.0f);

    if (stage == FrameStage::START)
        GameData::update();

    if (stage == FrameStage::RENDER_START) {
        Misc::disablePanoramablur();
        Visuals::colorWorld();
        Misc::fakePrime();
    	Animations::real();
        Visuals::NightMode();
    	Visuals::transparentWorld();
    }
    if (interfaces->engine->isInGame()) {
        Visuals::skybox(stage);
        Visuals::removeBlur(stage);
        Visuals::removeGrass(stage);
        Visuals::modifySmoke(stage);
        Visuals::playerModel(stage);
        Visuals::disablePostProcessing(stage);
        Visuals::removeVisualRecoil(stage);
        Visuals::applyZoom(stage);
        Misc::fixAnimationLOD(stage);
        Backtrack::update(stage);
        SkinChanger::run(stage);
    }
    hooks->client.callOriginal<void, 37>(stage);
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
        if (const auto entity = interfaces->entityList->getEntity(data.entityIndex); localPlayer && entity && entity->isPlayer()) {
            if (data.entityIndex == localPlayer->index())
                data.volume *= get(0) / 100.0f;
            else if (!entity->isOtherEnemy(localPlayer.get()))
                data.volume *= get(1) / 100.0f;
            else
                data.volume *= get(2) / 100.0f;
        }
    };

    modulateVolume([](int index) { return config->sound.players[index].masterVolume; });

    if (strstr(data.soundEntry, "Weapon") && strstr(data.soundEntry, "Single")) {
        modulateVolume([](int index) { return config->sound.players[index].weaponVolume; });
    } else if (config->misc.autoAccept && !strcmp(data.soundEntry, "UIPanorama.popup_accept_match_beep")) {
        memory->acceptMatch("");
        auto window = FindWindowW(L"Valve001", NULL);
        FLASHWINFO flash{ sizeof(FLASHWINFO), window, FLASHW_TRAY | FLASHW_TIMERNOFG, 0, 0 };
        FlashWindowEx(&flash);
        ShowWindow(window, SW_RESTORE);
    }
    data.volume = std::clamp(data.volume, 0.0f, 1.0f);
    hooks->sound.callOriginal<void, 5>(data);
}

static bool __stdcall shouldDrawFog() noexcept
{
    if constexpr (std::is_same_v<HookType, MinHook>) {
#ifdef _DEBUG
    // Check if we always get the same return address
    if (*static_cast<std::uint32_t*>(_ReturnAddress()) == 0x6274C084) {
        static const auto returnAddress = std::uintptr_t(_ReturnAddress());
        assert(returnAddress == std::uintptr_t(_ReturnAddress()));
    }
#endif

    if (*static_cast<std::uint32_t*>(_ReturnAddress()) != 0x6274C084)
        return hooks->clientMode.callOriginal<bool, 17>();
    }

    return !config->visuals.noFog;
}

static bool __stdcall shouldDrawViewModel() noexcept
{
    if (config->visuals.zoom && localPlayer && localPlayer->fov() < 45 && localPlayer->fovStart() < 45)
        return false;
    return hooks->clientMode.callOriginal<bool, 27>();
}

static void __stdcall lockCursor() noexcept
{
    if (gui->open)
        return interfaces->surface->unlockCursor();
    return hooks->surface.callOriginal<void, 67>();
}

static void __stdcall setDrawColor(int r, int g, int b, int a) noexcept
{
#ifdef _DEBUG
    // Check if we always get the same return address
    if (*static_cast<std::uint32_t*>(_ReturnAddress()) == 0x20244C8B) {
        static const auto returnAddress = std::uintptr_t(_ReturnAddress());
        assert(returnAddress == std::uintptr_t(_ReturnAddress()));
    }
    if (*reinterpret_cast<std::uint32_t*>(std::uintptr_t(_ReturnAddress()) + 6) == 0x01ACB7FF) {
        static const auto returnAddress = std::uintptr_t(_ReturnAddress());
        assert(returnAddress == std::uintptr_t(_ReturnAddress()));
    }
#endif

    if (config->visuals.noScopeOverlay && (*static_cast<std::uint32_t*>(_ReturnAddress()) == 0x20244C8B || *reinterpret_cast<std::uint32_t*>(std::uintptr_t(_ReturnAddress()) + 6) == 0x01ACB7FF))
        a = 0;
    hooks->surface.callOriginal<void, 15>(r, g, b, a);
}

static bool __stdcall fireEventClientSide(GameEvent* event) noexcept
{
    if (event) {
        switch (fnv::hashRuntime(event->getName())) {
	        case fnv::hash("bullet_impact"):
	            Visuals::bulletBeams(event);
	            break;
            default: break;
        }
    }
    return hooks->gameEventManager.callOriginal<bool, 9>(event);
}

struct ViewSetup {
    char _0x0000[16];
    __int32 x;
    __int32 x_old;
    __int32 y;
    __int32 y_old;
    __int32 width;
    __int32    width_old;
    __int32 height;
    __int32    height_old;
    char _0x0030[128];
    float fov;
    float fovViewmodel;
    Vector origin;
    Vector angles;
    float zNear;
    float farZ;
    float zNearViewmodel;
    float zFarViewmodel;
    float m_flAspectRatio;
    float m_flNearBlurDepth;
    float m_flNearFocusDepth;
    float m_flFarFocusDepth;
    float m_flFarBlurDepth;
    float m_flNearBlurRadius;
    float m_flFarBlurRadius;
    float m_nDoFQuality;
    __int32 m_nMotionBlurMode;
    char _0x0104[68];
    __int32 m_EdgeBlur;
};

static void __stdcall overrideView(ViewSetup* setup) noexcept
{
    if (localPlayer && !localPlayer->isScoped())
        setup->fov += config->visuals.fov;
    setup->farZ += config->visuals.farZ * 10;
	 if (config->misc.fakeDucking)
        setup->origin.z = localPlayer->getAbsOrigin().z + 64.f;
    hooks->clientMode.callOriginal<void, 18>(setup);
}

struct RenderableInfo {
    Entity* renderable;
    std::byte pad[18];
    uint16_t flags;
    uint16_t flags2;
};

static int __stdcall listLeavesInBox(const Vector& mins, const Vector& maxs, unsigned short* list, int listMax) noexcept
{
    if (std::uintptr_t(_ReturnAddress()) == memory->listLeaves) {
        if (const auto info = *reinterpret_cast<RenderableInfo**>(std::uintptr_t(_AddressOfReturnAddress()) + 0x14); info && info->renderable) {
            if (const auto ent = VirtualMethod::call<Entity*, 7>(info->renderable - 4); ent && ent->isPlayer()) {
                if (config->misc.disableModelOcclusion) {
                    // FIXME: sometimes players are rendered above smoke, maybe sort render list?
                    info->flags &= ~0x100;
                    info->flags2 |= 0x40;

                    constexpr float maxCoord = 16384.0f;
                    constexpr float minCoord = -maxCoord;
                    constexpr Vector min{ minCoord, minCoord, minCoord };
                    constexpr Vector max{ maxCoord, maxCoord, maxCoord };
                    return hooks->bspQuery.callOriginal<int, 6>(std::cref(min), std::cref(max), list, listMax);
                }
            }
        }
    }
    return hooks->bspQuery.callOriginal<int, 6>(std::cref(mins), std::cref(maxs), list, listMax);
}

static int __fastcall dispatchSound(SoundInfo& soundInfo) noexcept
{
    if (const char* soundName = interfaces->soundEmitter->getSoundName(soundInfo.soundIndex)) {
        auto modulateVolume = [&soundInfo](int(*get)(int)) {
            if (auto entity{ interfaces->entityList->getEntity(soundInfo.entityIndex) }; entity && entity->isPlayer()) {
                if (localPlayer && soundInfo.entityIndex == localPlayer->index())
                    soundInfo.volume *= get(0) / 100.0f;
                else if (!entity->isOtherEnemy(localPlayer.get()))
                    soundInfo.volume *= get(1) / 100.0f;
                else
                    soundInfo.volume *= get(2) / 100.0f;
            }
        };

        modulateVolume([](int index) { return config->sound.players[index].masterVolume; });

        if (!strcmp(soundName, "Player.DamageHelmetFeedback"))
            modulateVolume([](int index) { return config->sound.players[index].headshotVolume; });
        else if (strstr(soundName, "Step"))
            modulateVolume([](int index) { return config->sound.players[index].footstepVolume; });
        else if (strstr(soundName, "Chicken"))
            soundInfo.volume *= config->sound.chickenVolume / 100.0f;
    }
    soundInfo.volume = std::clamp(soundInfo.volume, 0.0f, 1.0f);
    return hooks->originalDispatchSound(soundInfo);
}

static int __stdcall render2dEffectsPreHud(int param) noexcept
{
    Visuals::applyScreenEffects();
    Visuals::hitEffect();
    return hooks->viewRender.callOriginal<int, 39>(param);
}

static const DemoPlaybackParameters* __stdcall getDemoPlaybackParameters() noexcept
{
    const auto params = hooks->engine.callOriginal<const DemoPlaybackParameters*, 218>();

#ifdef _DEBUG
    // Check if we always get the same return address
    if (*static_cast<std::uint64_t*>(_ReturnAddress()) == 0x79801F74C985C88B) {
        static const auto returnAddress = std::uintptr_t(_ReturnAddress());
        assert(returnAddress == std::uintptr_t(_ReturnAddress()));
    }
#endif

    if (params && config->misc.revealSuspect && *static_cast<std::uint64_t*>(_ReturnAddress()) != 0x79801F74C985C88B) { // client.dll : 8B C8 85 C9 74 1F 80 79 10 00 , there game decides whether to show overwatch panel
        static DemoPlaybackParameters customParams;
        customParams = *params;
        customParams.anonymousPlayerIdentity = false;
        return &customParams;
    }

    return params;
}

static bool __stdcall isPlayingDemo() noexcept
{
#ifdef _DEBUG
    // Check if we always get the same return address
    if (*static_cast<std::uintptr_t*>(_ReturnAddress()) == 0x0975C084
        && **reinterpret_cast<std::uintptr_t**>(std::uintptr_t(_AddressOfReturnAddress()) + 4) == 0x0C75C084) {
        static const auto returnAddress = std::uintptr_t(_ReturnAddress());
        assert(returnAddress == std::uintptr_t(_ReturnAddress()));
    }
#endif

    if (config->misc.revealMoney
        && *static_cast<uintptr_t*>(_ReturnAddress()) == 0x0975C084 // client.dll : 84 C0 75 09 38 05
        && **reinterpret_cast<uintptr_t**>(uintptr_t(_AddressOfReturnAddress()) + 4) == 0x0C75C084) { // client.dll : 84 C0 75 0C 5B
        return true;
    }
    return hooks->engine.callOriginal<bool, 82>();
}

static void __stdcall updateColorCorrectionWeights() noexcept
{
    hooks->clientMode.callOriginal<void, 58>();

    if (const auto& cfg = config->visuals.colorCorrection; cfg.enabled) {
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + 0x498) = cfg.blue;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + 0x4A0) = cfg.red;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + 0x4A8) = cfg.mono;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + 0x4B0) = cfg.saturation;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + 0x4C0) = cfg.ghost;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + 0x4C8) = cfg.green;
        *reinterpret_cast<float*>(std::uintptr_t(memory->clientMode) + 0x4D0) = cfg.yellow;
    }

    if (config->visuals.noScopeOverlay)
        *memory->vignette = 0.0f;
}

static float __stdcall getScreenAspectRatio(int width, int height) noexcept
{
    if (config->misc.aspectratio)
        return config->misc.aspectratio;
    return hooks->engine.callOriginal<float, 101>(width, height);
}

static void __stdcall renderSmokeOverlay(bool update) noexcept
{
    if (config->visuals.noSmoke || config->visuals.wireframeSmoke)
        *reinterpret_cast<float*>(std::uintptr_t(memory->viewRender) + 0x588) = 0.0f;
    else
        hooks->viewRender.callOriginal<void, 41>(update);
}

Hooks::Hooks(HMODULE module) noexcept
{
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

    this->module = module;

    // interfaces and memory shouldn't be initialized in wndProc because they show MessageBox on error which would cause deadlock
    interfaces = std::make_unique<const Interfaces>();
    memory = std::make_unique<const Memory>();

    window = FindWindowW(L"Valve001", nullptr);
    originalWndProc = WNDPROC(SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(wndProc)));

    Tickbase::tick = std::make_unique<Tickbase::Tick>();//put this on once on "wndProc"
}

void WriteUsercmd(void* buf, UserCmd* in, UserCmd* out)
{
    static DWORD WriteUsercmdF = (DWORD)memory->WriteUsercmd;

    __asm
    {
        mov ecx, buf
        mov edx, in
        push out
        call WriteUsercmdF
        add esp, 4
    }
}

static bool __fastcall WriteUsercmdDeltaToBuffer(void* ecx, void* edx, int slot, void* buffer, int from, int to, bool isnewcommand) noexcept
{
    auto original = hooks->client.getOriginal<bool, 24>(slot, buffer, from, to, isnewcommand);
    if (_ReturnAddress() == memory->WriteUsercmdDeltaToBufferReturn || Tickbase::tick->tickshift <= 0 || !memory->clientState)
        return original(ecx, slot, buffer, from, to, isnewcommand);

    if (from != -1)
        return true;

    int* numBackupCommands = (int*)(reinterpret_cast <uintptr_t> (buffer) - 0x30);
    int* numNewCommands = (int*)(reinterpret_cast <uintptr_t> (buffer) - 0x2C);

    int32_t newcommands = *numNewCommands;

    int nextcommmand = memory->clientState->lastOutgoingCommand + memory->clientState->chokedCommands + 1;
    int totalcommands = std::min(Tickbase::tick->tickshift, Tickbase::tick->maxUsercmdProcessticks);
    Tickbase::tick->tickshift = 0;

    from = -1;
    *numNewCommands = totalcommands;
    *numBackupCommands = 0;

    for (to = nextcommmand - newcommands + 1; to <= nextcommmand; to++)
    {
        if (!(original(ecx, slot, buffer, from, to, true)))
            return false;

        from = to;
    }

    UserCmd* lastRealCmd = memory->input->GetUserCmd(slot, from);
    UserCmd fromcmd;

    if (lastRealCmd)
        fromcmd = *lastRealCmd;

    UserCmd tocmd = fromcmd;
    tocmd.tickCount += 200;
    tocmd.commandNumber++;

    for (int i = newcommands; i <= totalcommands; i++)
    {
        WriteUsercmd(buffer, &tocmd, &fromcmd);
        fromcmd = tocmd;
        tocmd.commandNumber++;
        tocmd.tickCount++;
    }

    return true;
}

void Hooks::install() noexcept
{
    SkinChanger::initializeKits();

    originalPresent = **reinterpret_cast<decltype(originalPresent)**>(memory->present);
    **reinterpret_cast<decltype(present)***>(memory->present) = present;
    originalReset = **reinterpret_cast<decltype(originalReset)**>(memory->reset);
    **reinterpret_cast<decltype(reset)***>(memory->reset) = reset;

    if constexpr (std::is_same_v<HookType, MinHook>)
        MH_Initialize();

    bspQuery.init(interfaces->engine->getBSPTreeQuery());
    client.init(interfaces->client);
    clientMode.init(memory->clientMode);
    engine.init(interfaces->engine);
    modelRender.init(interfaces->modelRender);
    panel.init(interfaces->panel);
    sound.init(interfaces->sound);
    surface.init(interfaces->surface);
    svCheats.init(interfaces->cvar->findVar("sv_cheats"));
    viewRender.init(memory->viewRender);
	gameEventManager.init(interfaces->gameEventManager);

    bspQuery.hookAt(6, listLeavesInBox);
    client.hookAt(24, WriteUsercmdDeltaToBuffer);
    client.hookAt(37, frameStageNotify);
    clientMode.hookAt(17, shouldDrawFog);
    clientMode.hookAt(18, overrideView);
    clientMode.hookAt(24, createMove);
    clientMode.hookAt(27, shouldDrawViewModel);
    clientMode.hookAt(35, getViewModelFov);
    clientMode.hookAt(44, doPostScreenEffects);
    clientMode.hookAt(58, updateColorCorrectionWeights);
    engine.hookAt(82, isPlayingDemo);
    engine.hookAt(101, getScreenAspectRatio);
    engine.hookAt(218, getDemoPlaybackParameters);
    gameEventManager.hookAt(9, fireEventClientSide);
    modelRender.hookAt(21, drawModelExecute);
    panel.hookAt(41, paintTraverse);
    sound.hookAt(5, emitSound);
    surface.hookAt(15, setDrawColor);
    surface.hookAt(67, lockCursor);
    svCheats.hookAt(13, svCheatsGetBool);
    viewRender.hookAt(39, render2dEffectsPreHud);
    viewRender.hookAt(41, renderSmokeOverlay);

    if (DWORD oldProtection; VirtualProtect(memory->dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        originalDispatchSound = decltype(originalDispatchSound)(uintptr_t(memory->dispatchSound + 1) + *memory->dispatchSound);
        *memory->dispatchSound = uintptr_t(dispatchSound) - uintptr_t(memory->dispatchSound + 1);
        VirtualProtect(memory->dispatchSound, 4, oldProtection, nullptr);
    }

    if constexpr (std::is_same_v<HookType, MinHook>)
        MH_EnableHook(MH_ALL_HOOKS);
}

extern "C" BOOL WINAPI _CRT_INIT(HMODULE module, DWORD reason, LPVOID reserved);

static DWORD WINAPI unload(HMODULE module) noexcept
{
    Sleep(100);

    interfaces->inputSystem->enableInput(true);
    eventListener->remove();

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    _CRT_INIT(module, DLL_PROCESS_DETACH, nullptr);

    FreeLibraryAndExitThread(module, 0);
}

void Hooks::uninstall() noexcept
{
    if constexpr (std::is_same_v<HookType, MinHook>) {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }

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
    networkChannel.restore();
    extraHook.restore();
	
    netvars->restore();

    Glow::clearCustomObjects();

    SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(originalWndProc));
    **reinterpret_cast<void***>(memory->present) = originalPresent;
    **reinterpret_cast<void***>(memory->reset) = originalReset;

    if (DWORD oldProtection; VirtualProtect(memory->dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        *memory->dispatchSound = uintptr_t(originalDispatchSound) - uintptr_t(memory->dispatchSound + 1);
        VirtualProtect(memory->dispatchSound, 4, oldProtection, nullptr);
    }

    if (HANDLE thread = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(unload), module, 0, nullptr))
        CloseHandle(thread);
}
