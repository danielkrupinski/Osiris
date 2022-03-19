#include <charconv>
#include <functional>
#include <string>

#include "imgui/imgui.h"

#ifdef _WIN32
#include <intrin.h>
#include <Windows.h>
#include <Psapi.h>

#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "MinHook/MinHook.h"
#elif __linux__
#include <sys/mman.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#endif

#include "Config.h"
#include "EventListener.h"
#include "GameData.h"
#include "GUI.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "Memory.h"

#include "Hacks/Aimbot.h"
#include "Hacks/AntiAim.h"
#include "Hacks/Backtrack.h"
#include "Hacks/Chams.h"
#include "Hacks/EnginePrediction.h"
#include "Hacks/StreamProofESP.h"
#include "Hacks/Glow.h"
#include "Hacks/Misc.h"
#include "Hacks/Sound.h"
#include "Hacks/Triggerbot.h"
#include "Hacks/Visuals.h"

#include "InventoryChanger/InventoryChanger.h"

#include "SDK/ClientClass.h"
#include "SDK/Cvar.h"
#include "SDK/Engine.h"
#include "SDK/Entity.h"
#include "SDK/EntityList.h"
#include "SDK/FrameStage.h"
#include "SDK/GameEvent.h"
#include "SDK/GameUI.h"
#include "SDK/GlobalVars.h"
#include "SDK/InputSystem.h"
#include "SDK/ItemSchema.h"
#include "SDK/LocalPlayer.h"
#include "SDK/MaterialSystem.h"
#include "SDK/ModelRender.h"
#include "SDK/Platform.h"
#include "SDK/RenderContext.h"
#include "SDK/SoundInfo.h"
#include "SDK/SoundEmitter.h"
#include "SDK/StudioRender.h"
#include "SDK/Surface.h"
#include "SDK/UserCmd.h"
#include "SDK/UserMessages.h"

#ifdef _WIN32

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT __stdcall wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    [[maybe_unused]] static const auto once = [](HWND window) noexcept {
        Netvars::init();
        EventListener::init();

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(window);
        config = std::make_unique<Config>();
        gui = std::make_unique<GUI>();

        hooks->install();

        return true;
    }(window);

    ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam);
    interfaces->inputSystem->enableInput(!gui->isOpen());

    return CallWindowProcW(hooks->originalWndProc, window, msg, wParam, lParam);
}

static HRESULT __stdcall reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    InventoryChanger::clearItemIconTextures();
    GameData::clearTextures();
    return hooks->originalReset(device, params);
}

#endif

#ifdef _WIN32
static HRESULT __stdcall present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
{
    [[maybe_unused]] static bool imguiInit{ ImGui_ImplDX9_Init(device) };

    if (config->loadScheduledFonts())
        ImGui_ImplDX9_DestroyFontsTexture();

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
#else
static void swapWindow(SDL_Window * window) noexcept
{
    static const auto _ = ImGui_ImplSDL2_InitForOpenGL(window, nullptr);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
#endif
    ImGui::NewFrame();

    if (const auto& displaySize = ImGui::GetIO().DisplaySize; displaySize.x > 0.0f && displaySize.y > 0.0f) {
        StreamProofESP::render();
        Misc::purchaseList();
        Misc::noscopeCrosshair(ImGui::GetBackgroundDrawList());
        Misc::recoilCrosshair(ImGui::GetBackgroundDrawList());
        Misc::drawOffscreenEnemies(ImGui::GetBackgroundDrawList());
        Misc::drawBombTimer();
        Misc::spectatorList();
        Visuals::hitMarker(nullptr, ImGui::GetBackgroundDrawList());
        Visuals::drawMolotovHull(ImGui::GetBackgroundDrawList());
        Misc::watermark();

        Aimbot::updateInput();
        Visuals::updateInput();
        StreamProofESP::updateInput();
        Misc::updateInput();
        Triggerbot::updateInput();
        Chams::updateInput();
        Glow::updateInput();

        gui->handleToggle();

        if (gui->isOpen())
            gui->render();
    }

    ImGui::EndFrame();
    ImGui::Render();

#ifdef _WIN32
    if (device->BeginScene() == D3D_OK) {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }
#else
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

    GameData::clearUnusedAvatars();
    InventoryChanger::clearUnusedItemIconTextures();

#ifdef _WIN32
    return hooks->originalPresent(device, src, dest, windowOverride, dirtyRegion);
#else
    hooks->swapWindow(window);
#endif
}

static bool STDCALL_CONV createMove(LINUX_ARGS(void* thisptr,) float inputSampleTime, UserCmd* cmd) noexcept
{
    auto result = hooks->clientMode.callOriginal<bool, WIN32_LINUX(24, 25)>(inputSampleTime, cmd);

    if (!cmd->commandNumber)
        return result;

#ifdef _WIN32
    // bool& sendPacket = *reinterpret_cast<bool*>(*reinterpret_cast<std::uintptr_t*>(FRAME_ADDRESS()) - 0x1C);
    // since 19.02.2022 game update sendPacket is no longer on stack
    bool sendPacket = true;
#else
    bool dummy;
    bool& sendPacket = dummy;
#endif

    static auto previousViewAngles{ cmd->viewangles };
    const auto currentViewAngles{ cmd->viewangles };

    memory->globalVars->serverTime(cmd);
    Misc::nadePredict();
    Misc::antiAfkKick(cmd);
    Misc::fastStop(cmd);
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
    Misc::fixTabletSignal();
    Misc::slowwalk(cmd);

    EnginePrediction::run(cmd);

    Aimbot::run(cmd);
    Triggerbot::run(cmd);
    Backtrack::run(cmd);
    Misc::edgejump(cmd);
    Misc::moonwalk(cmd);
    Misc::fastPlant(cmd);

    if (!(cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2))) {
        Misc::chokePackets(sendPacket);
        AntiAim::run(cmd, previousViewAngles, currentViewAngles, sendPacket);
    }

    auto viewAnglesDelta{ cmd->viewangles - previousViewAngles };
    viewAnglesDelta.normalize();
    viewAnglesDelta.x = std::clamp(viewAnglesDelta.x, -Misc::maxAngleDelta(), Misc::maxAngleDelta());
    viewAnglesDelta.y = std::clamp(viewAnglesDelta.y, -Misc::maxAngleDelta(), Misc::maxAngleDelta());

    cmd->viewangles = previousViewAngles + viewAnglesDelta;

    cmd->viewangles.normalize();
    Misc::fixMovement(cmd, currentViewAngles.y);

    cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
    cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
    cmd->viewangles.z = 0.0f;
    cmd->forwardmove = std::clamp(cmd->forwardmove, -450.0f, 450.0f);
    cmd->sidemove = std::clamp(cmd->sidemove, -450.0f, 450.0f);

    previousViewAngles = cmd->viewangles;

    return false;
}

static void STDCALL_CONV doPostScreenEffects(LINUX_ARGS(void* thisptr,) void* param) noexcept
{
    if (interfaces->engine->isInGame()) {
        Visuals::thirdperson();
        Visuals::inverseRagdollGravity();
        Visuals::reduceFlashEffect();
        Visuals::updateBrightness();
        Visuals::remove3dSky();
        Glow::render();
    }
    hooks->clientMode.callOriginal<void, WIN32_LINUX(44, 45)>(param);
}

static float STDCALL_CONV getViewModelFov(LINUX_ARGS(void* thisptr)) noexcept
{
    float additionalFov = Visuals::viewModelFov();
    if (localPlayer) {
        if (const auto activeWeapon = localPlayer->getActiveWeapon(); activeWeapon && activeWeapon->getClientClass()->classId == ClassId::Tablet)
            additionalFov = 0.0f;
    }

    return hooks->clientMode.callOriginal<float, WIN32_LINUX(35, 36)>() + additionalFov;
}

static void STDCALL_CONV drawModelExecute(LINUX_ARGS(void* thisptr,) void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    if (interfaces->studioRender->isForcedMaterialOverride())
        return hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);

    if (Visuals::removeHands(info.model->name) || Visuals::removeSleeves(info.model->name) || Visuals::removeWeapons(info.model->name))
        return;

    if (static Chams chams; !chams.render(ctx, state, info, customBoneToWorld))
        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);

    interfaces->studioRender->forcedMaterialOverride(nullptr);
}

static bool FASTCALL_CONV svCheatsGetBool(void* _this) noexcept
{
    if (RETURN_ADDRESS() == memory->cameraThink && Visuals::isThirdpersonOn())
        return true;

    return hooks->svCheats.getOriginal<bool, WIN32_LINUX(13, 16)>()(_this);
}

static void STDCALL_CONV frameStageNotify(LINUX_ARGS(void* thisptr,) FrameStage stage) noexcept
{
    [[maybe_unused]] static auto backtrackInit = (Backtrack::init(), false);

    if (interfaces->engine->isConnected() && !interfaces->engine->isInGame())
        Misc::changeName(true, nullptr, 0.0f);

    if (stage == FrameStage::START)
        GameData::update();

    if (stage == FrameStage::RENDER_START) {
        Misc::preserveKillfeed();
        Misc::disablePanoramablur();
        Visuals::colorWorld();
        Misc::updateEventListeners();
        Visuals::updateEventListeners();
    }
    if (interfaces->engine->isInGame()) {
        Visuals::skybox(stage);
        Visuals::removeBlur(stage);
        Misc::oppositeHandKnife(stage);
        Visuals::removeGrass(stage);
        Visuals::modifySmoke(stage);
        Visuals::disablePostProcessing(stage);
        Visuals::removeVisualRecoil(stage);
        Visuals::applyZoom(stage);
        Misc::fixAnimationLOD(stage);
        Backtrack::update(stage);
    }
    InventoryChanger::run(stage);

    hooks->client.callOriginal<void, 37>(stage);
}

static int STDCALL_CONV emitSound(LINUX_ARGS(void* thisptr,) void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const Vector& origin, const Vector& direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams) noexcept
{
    Sound::modulateSound(soundEntry, entityIndex, volume);
    Misc::autoAccept(soundEntry);

    volume = std::clamp(volume, 0.0f, 1.0f);
    return hooks->sound.callOriginal<int, WIN32_LINUX(5, 6)>(filter, entityIndex, channel, soundEntry, soundEntryHash, sample, volume, seed, soundLevel, flags, pitch, std::cref(origin), std::cref(direction), utlVecOrigins, updatePositions, soundtime, speakerentity, soundParams);
}

static bool STDCALL_CONV shouldDrawFog(LINUX_ARGS(void* thisptr)) noexcept
{
#ifdef _WIN32
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
#endif
    
    return !Visuals::shouldRemoveFog();
}

static bool STDCALL_CONV shouldDrawViewModel(LINUX_ARGS(void* thisptr)) noexcept
{
    if (Visuals::isZoomOn() && localPlayer && localPlayer->fov() < 45 && localPlayer->fovStart() < 45)
        return false;
    return hooks->clientMode.callOriginal<bool, WIN32_LINUX(27, 28)>();
}

static void STDCALL_CONV lockCursor() noexcept
{
    if (gui->isOpen())
        return interfaces->surface->unlockCursor();
    return hooks->surface.callOriginal<void, 67>();
}

static void STDCALL_CONV setDrawColor(LINUX_ARGS(void* thisptr,) int r, int g, int b, int a) noexcept
{
    if (Visuals::shouldRemoveScopeOverlay() && (RETURN_ADDRESS() == memory->scopeDust || RETURN_ADDRESS() == memory->scopeArc))
        a = 0;
    hooks->surface.callOriginal<void, WIN32_LINUX(15, 14)>(r, g, b, a);
}

struct ViewSetup {
    PAD(172);
    void* csm;
    float fov;
    PAD(32);
    float farZ;
};

static void STDCALL_CONV overrideView(LINUX_ARGS(void* thisptr,) ViewSetup* setup) noexcept
{
    if (localPlayer && !localPlayer->isScoped())
        setup->fov += Visuals::fov();
    setup->farZ += Visuals::farZ() * 10;
    hooks->clientMode.callOriginal<void, WIN32_LINUX(18, 19)>(setup);
}

struct RenderableInfo {
    Entity* renderable;
    std::byte pad[18];
    uint16_t flags;
    uint16_t flags2;
};

static int STDCALL_CONV listLeavesInBox(LINUX_ARGS(void* thisptr, ) const Vector& mins, const Vector& maxs, unsigned short* list, int listMax) noexcept
{
    if (Misc::shouldDisableModelOcclusion() && RETURN_ADDRESS() == memory->insertIntoTree) {
        if (const auto info = *reinterpret_cast<RenderableInfo**>(FRAME_ADDRESS() + WIN32_LINUX(0x18, 0x10 + 0x948)); info && info->renderable) {
            if (const auto ent = VirtualMethod::call<Entity*, WIN32_LINUX(7, 8)>(info->renderable - sizeof(std::uintptr_t)); ent && ent->isPlayer()) {
                constexpr float maxCoord = 16384.0f;
                constexpr float minCoord = -maxCoord;
                constexpr Vector min{ minCoord, minCoord, minCoord };
                constexpr Vector max{ maxCoord, maxCoord, maxCoord };
                return hooks->bspQuery.callOriginal<int, 6>(std::cref(min), std::cref(max), list, listMax);
            }
        }
    }

    return hooks->bspQuery.callOriginal<int, 6>(std::cref(mins), std::cref(maxs), list, listMax);
}

static int FASTCALL_CONV dispatchSound(SoundInfo& soundInfo) noexcept
{
    if (const char* soundName = interfaces->soundEmitter->getSoundName(soundInfo.soundIndex)) {
        Sound::modulateSound(soundName, soundInfo.entityIndex, soundInfo.volume);
        soundInfo.volume = std::clamp(soundInfo.volume, 0.0f, 1.0f);
    }
    return hooks->originalDispatchSound(soundInfo);
}

static void STDCALL_CONV render2dEffectsPreHud(LINUX_ARGS(void* thisptr,) void* viewSetup) noexcept
{
    Visuals::applyScreenEffects();
    Visuals::hitEffect();
    hooks->viewRender.callOriginal<void, WIN32_LINUX(39, 40)>(viewSetup);
}

static const DemoPlaybackParameters* STDCALL_CONV getDemoPlaybackParameters(LINUX_ARGS(void* thisptr)) noexcept
{
    const auto params = hooks->engine.callOriginal<const DemoPlaybackParameters*, WIN32_LINUX(218, 219)>();

    if (params && Misc::shouldRevealSuspect() && RETURN_ADDRESS() != memory->demoFileEndReached) {
        static DemoPlaybackParameters customParams;
        customParams = *params;
        customParams.anonymousPlayerIdentity = false;
        return &customParams;
    }

    return params;
}

static bool STDCALL_CONV isPlayingDemo(LINUX_ARGS(void* thisptr)) noexcept
{
    if (Misc::shouldRevealMoney() && RETURN_ADDRESS() == memory->demoOrHLTV && *reinterpret_cast<std::uintptr_t*>(FRAME_ADDRESS() + WIN32_LINUX(8, 24)) == memory->money)
        return true;

    return hooks->engine.callOriginal<bool, 82>();
}

static void STDCALL_CONV updateColorCorrectionWeights(LINUX_ARGS(void* thisptr)) noexcept
{
    hooks->clientMode.callOriginal<void, WIN32_LINUX(58, 61)>();

    Visuals::performColorCorrection();
    if (Visuals::shouldRemoveScopeOverlay())
        *memory->vignette = 0.0f;
}

static float STDCALL_CONV getScreenAspectRatio(LINUX_ARGS(void* thisptr,) int width, int height) noexcept
{
    if (Misc::aspectRatio() != 0.0f)
        return Misc::aspectRatio();
    return hooks->engine.callOriginal<float, 101>(width, height);
}

static void STDCALL_CONV renderSmokeOverlay(LINUX_ARGS(void* thisptr,) bool update) noexcept
{
    if (Visuals::shouldRemoveSmoke() || Visuals::isSmokeWireframe())
        *reinterpret_cast<float*>(std::uintptr_t(memory->viewRender) + WIN32_LINUX(0x588, 0x648)) = 0.0f;
    else
        hooks->viewRender.callOriginal<void, WIN32_LINUX(41, 42)>(update);
}

static double STDCALL_CONV getArgAsNumber(LINUX_ARGS(void* thisptr,) void* params, int index) noexcept
{
    const auto result = hooks->panoramaMarshallHelper.callOriginal<double, 5>(params, index);
    InventoryChanger::getArgAsNumberHook(static_cast<int>(result), RETURN_ADDRESS());
    return result;
}

static const char* STDCALL_CONV getArgAsString(LINUX_ARGS(void* thisptr,) void* params, int index) noexcept
{
    const auto result = hooks->panoramaMarshallHelper.callOriginal<const char*, 7>(params, index);

    if (result)
        InventoryChanger::getArgAsStringHook(result, RETURN_ADDRESS());

    return result;
}

static bool STDCALL_CONV equipItemInLoadout(LINUX_ARGS(void* thisptr, ) Team team, int slot, std::uint64_t itemID, bool swap) noexcept
{
    InventoryChanger::onItemEquip(team, slot, itemID);
    return hooks->inventoryManager.callOriginal<bool, WIN32_LINUX(20, 21)>(team, slot, itemID, swap);
}

static void STDCALL_CONV soUpdated(LINUX_ARGS(void* thisptr, ) SOID owner, SharedObject* object, int event) noexcept
{
    InventoryChanger::onSoUpdated(object);
    hooks->inventory.callOriginal<void, 1>(owner, object, event);
}

static bool STDCALL_CONV dispatchUserMessage(LINUX_ARGS(void* thisptr, ) UserMessageType type, int passthroughFlags, int size, const void* data) noexcept
{
    if (type == UserMessageType::Text)
        InventoryChanger::onUserTextMsg(data, size);
    else if (type == UserMessageType::VoteStart)
        Misc::onVoteStart(data, size);
    else if (type == UserMessageType::VotePass)
        Misc::onVotePass();
    else if (type == UserMessageType::VoteFailed)
        Misc::onVoteFailed();
    
    return hooks->client.callOriginal<bool, 38>(type, passthroughFlags, size, data);
}

#ifdef _WIN32

static void* STDCALL_CONV allocKeyValuesMemory(LINUX_ARGS(void* thisptr, ) int size) noexcept
{
    if (const auto returnAddress = RETURN_ADDRESS(); returnAddress == memory->keyValuesAllocEngine || returnAddress == memory->keyValuesAllocClient)
        return nullptr;
    return hooks->keyValuesSystem.callOriginal<void*, 1>(size);
}

Hooks::Hooks(HMODULE moduleHandle) noexcept : moduleHandle{ moduleHandle }
{
#ifndef __MINGW32__
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#endif

    // interfaces and memory shouldn't be initialized in wndProc because they show MessageBox on error which would cause deadlock
    interfaces = std::make_unique<const Interfaces>();
    memory = std::make_unique<const Memory>();

    window = FindWindowW(L"Valve001", nullptr);
    originalWndProc = WNDPROC(SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(&wndProc)));
}

#endif

void Hooks::install() noexcept
{
#ifdef _WIN32
    originalPresent = **reinterpret_cast<decltype(originalPresent)**>(memory->present);
    **reinterpret_cast<decltype(present)***>(memory->present) = present;
    originalReset = **reinterpret_cast<decltype(originalReset)**>(memory->reset);
    **reinterpret_cast<decltype(reset)***>(memory->reset) = reset;

    if constexpr (std::is_same_v<HookType, MinHook>)
        MH_Initialize();
#else
    ImGui_ImplOpenGL3_Init();

    swapWindow = *reinterpret_cast<decltype(swapWindow)*>(memory->swapWindow);
    *reinterpret_cast<decltype(::swapWindow)**>(memory->swapWindow) = ::swapWindow;

#endif
    
    bspQuery.init(interfaces->engine->getBSPTreeQuery());
    bspQuery.hookAt(6, &listLeavesInBox);

    client.init(interfaces->client);
    client.hookAt(37, &frameStageNotify);
    client.hookAt(38, &dispatchUserMessage);

    clientMode.init(memory->clientMode);
    clientMode.hookAt(WIN32_LINUX(17, 18), &shouldDrawFog);
    clientMode.hookAt(WIN32_LINUX(18, 19), &overrideView);
    clientMode.hookAt(WIN32_LINUX(24, 25), &createMove);
    clientMode.hookAt(WIN32_LINUX(27, 28), &shouldDrawViewModel);
    clientMode.hookAt(WIN32_LINUX(35, 36), &getViewModelFov);
    clientMode.hookAt(WIN32_LINUX(44, 45), &doPostScreenEffects);
    clientMode.hookAt(WIN32_LINUX(58, 61), &updateColorCorrectionWeights);

    engine.init(interfaces->engine);
    engine.hookAt(82, &isPlayingDemo);
    engine.hookAt(101, &getScreenAspectRatio);
#ifdef _WIN32
    keyValuesSystem.init(memory->keyValuesSystem);
    keyValuesSystem.hookAt(1, &allocKeyValuesMemory);
#endif
    engine.hookAt(WIN32_LINUX(218, 219), &getDemoPlaybackParameters);

    inventory.init(memory->inventoryManager->getLocalInventory());
    inventory.hookAt(1, &soUpdated);

    inventoryManager.init(memory->inventoryManager);
    inventoryManager.hookAt(WIN32_LINUX(20, 21), &equipItemInLoadout);

    modelRender.init(interfaces->modelRender);
    modelRender.hookAt(21, &drawModelExecute);

    panoramaMarshallHelper.init(memory->panoramaMarshallHelper);
    panoramaMarshallHelper.hookAt(5, &getArgAsNumber);
    panoramaMarshallHelper.hookAt(7, &getArgAsString);

    sound.init(interfaces->sound);
    sound.hookAt(WIN32_LINUX(5, 6), &emitSound);

    surface.init(interfaces->surface);
    surface.hookAt(WIN32_LINUX(15, 14), &setDrawColor);
    
    svCheats.init(interfaces->cvar->findVar("sv_cheats"));
    svCheats.hookAt(WIN32_LINUX(13, 16), &svCheatsGetBool);

    viewRender.init(memory->viewRender);
    viewRender.hookAt(WIN32_LINUX(39, 40), &render2dEffectsPreHud);
    viewRender.hookAt(WIN32_LINUX(41, 42), &renderSmokeOverlay);

#ifdef _WIN32
    if (DWORD oldProtection; VirtualProtect(memory->dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection)) {
#else
    if (const auto addressPageAligned = std::uintptr_t(memory->dispatchSound) - std::uintptr_t(memory->dispatchSound) % sysconf(_SC_PAGESIZE);
        mprotect((void*)addressPageAligned, 4, PROT_READ | PROT_WRITE | PROT_EXEC) == 0) {
#endif
        originalDispatchSound = decltype(originalDispatchSound)(uintptr_t(memory->dispatchSound + 1) + *memory->dispatchSound);
        *memory->dispatchSound = uintptr_t(&dispatchSound) - uintptr_t(memory->dispatchSound + 1);
#ifdef _WIN32
        VirtualProtect(memory->dispatchSound, 4, oldProtection, nullptr);
#endif
    }

#ifdef _WIN32
    surface.hookAt(67, &lockCursor);

    if constexpr (std::is_same_v<HookType, MinHook>)
        MH_EnableHook(MH_ALL_HOOKS);
#endif
}

#ifdef _WIN32

extern "C" BOOL WINAPI _CRT_INIT(HMODULE moduleHandle, DWORD reason, LPVOID reserved);

static DWORD WINAPI unload(HMODULE moduleHandle) noexcept
{
    Sleep(100);

    interfaces->inputSystem->enableInput(true);
    EventListener::remove();

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    _CRT_INIT(moduleHandle, DLL_PROCESS_DETACH, nullptr);

    FreeLibraryAndExitThread(moduleHandle, 0);
}

#endif

void Hooks::uninstall() noexcept
{
    Misc::updateEventListeners(true);
    Visuals::updateEventListeners(true);

#ifdef _WIN32
    if constexpr (std::is_same_v<HookType, MinHook>) {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }
#endif

    bspQuery.restore();
    client.restore();
    clientMode.restore();
    engine.restore();
    inventory.restore();
    inventoryManager.restore();
    modelRender.restore();
    panoramaMarshallHelper.restore();
    sound.restore();
    surface.restore();
    svCheats.restore();
    viewRender.restore();

    Netvars::restore();

    Glow::clearCustomObjects();
    InventoryChanger::clearInventory();

#ifdef _WIN32
    keyValuesSystem.restore();

    SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(originalWndProc));
    **reinterpret_cast<void***>(memory->present) = originalPresent;
    **reinterpret_cast<void***>(memory->reset) = originalReset;

    if (DWORD oldProtection; VirtualProtect(memory->dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        *memory->dispatchSound = uintptr_t(originalDispatchSound) - uintptr_t(memory->dispatchSound + 1);
        VirtualProtect(memory->dispatchSound, 4, oldProtection, nullptr);
    }

    if (HANDLE thread = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(unload), moduleHandle, 0, nullptr))
        CloseHandle(thread);
#else
    *reinterpret_cast<decltype(pollEvent)*>(memory->pollEvent) = pollEvent;
    *reinterpret_cast<decltype(swapWindow)*>(memory->swapWindow) = swapWindow;
#endif
}

void Hooks::callOriginalDrawModelExecute(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
}

#ifndef _WIN32

static int pollEvent(SDL_Event* event) noexcept
{
    [[maybe_unused]] static const auto once = []() noexcept {
        Netvars::init();
        EventListener::init();

        ImGui::CreateContext();
        config = std::make_unique<Config>();

        gui = std::make_unique<GUI>();

        hooks->install();

        return true;
    }();

    const auto result = hooks->pollEvent(event);

    if (result && ImGui_ImplSDL2_ProcessEvent(event) && gui->isOpen())
        event->type = 0;

    return result;
}

Hooks::Hooks() noexcept
{
    interfaces = std::make_unique<const Interfaces>();
    memory = std::make_unique<const Memory>();

    pollEvent = *reinterpret_cast<decltype(pollEvent)*>(memory->pollEvent);
    *reinterpret_cast<decltype(::pollEvent)**>(memory->pollEvent) = ::pollEvent;
}

#endif
