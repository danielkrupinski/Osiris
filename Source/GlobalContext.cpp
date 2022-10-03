#include "GlobalContext.h"

#ifdef _WIN32
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>
#else
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#endif

#include "EventListener.h"
#include "GameData.h"
#include "GUI.h"
#include "Hooks.h"
#include "InventoryChanger/InventoryChanger.h"
#include "Memory.h"
#include "Hacks/Aimbot.h"
#include "Hacks/AntiAim.h"
#include "Hacks/Backtrack.h"
#include "Hacks/Chams.h"
#include "Hacks/EnginePrediction.h"
#include "Hacks/Glow.h"
#include "Hacks/Misc.h"
#include "Hacks/Sound.h"
#include "Hacks/StreamProofESP.h"
#include "Hacks/Triggerbot.h"
#include "Hacks/Visuals.h"
#include "SDK/ClientClass.h"
#include "SDK/Constants/ClassId.h"
#include "SDK/Constants/FrameStage.h"
#include "SDK/Constants/UserMessages.h"
#include "SDK/Engine.h"
#include "SDK/Entity.h"
#include "SDK/EntityList.h"
#include "SDK/GlobalVars.h"
#include "SDK/InputSystem.h"
#include "SDK/LocalPlayer.h"
#include "SDK/ModelRender.h"
#include "SDK/Recv.h"
#include "SDK/SoundEmitter.h"
#include "SDK/SoundInfo.h"
#include "SDK/StudioRender.h"
#include "SDK/Surface.h"
#include "SDK/UserCmd.h"
#include "SDK/ViewSetup.h"

#include "Interfaces.h"

bool GlobalContext::createMoveHook(float inputSampleTime, UserCmd* cmd)
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
    Misc::nadePredict(*interfaces);
    Misc::antiAfkKick(cmd);
    Misc::fastStop(cmd);
    Misc::prepareRevolver(*engineInterfaces->engine, *memory, cmd);
    Visuals::removeShadows(*interfaces);
    Misc::runReportbot(*engineInterfaces->engine, *clientInterfaces, *interfaces, *memory);
    Misc::bunnyHop(cmd);
    Misc::autoStrafe(cmd);
    Misc::removeCrouchCooldown(cmd);
    Misc::autoPistol(*memory, cmd);
    Misc::autoReload(cmd);
    Misc::updateClanTag(*memory);
    Misc::fakeBan(*engineInterfaces->engine, *interfaces, *memory);
    Misc::stealNames(*engineInterfaces->engine, *clientInterfaces, *interfaces, *memory);
    Misc::revealRanks(*clientInterfaces, cmd);
    Misc::quickReload(*clientInterfaces, *interfaces, cmd);
    Misc::fixTabletSignal();
    Misc::slowwalk(cmd);

    EnginePrediction::run(*clientInterfaces, *memory, cmd);

    Aimbot::run(*engineInterfaces, *clientInterfaces, *interfaces, *config, *memory, cmd);
    Triggerbot::run(*engineInterfaces->engineTrace, *interfaces, *memory, *config, cmd);
    Backtrack::run(*clientInterfaces, *engineInterfaces, *interfaces, *memory, cmd);
    Misc::edgejump(cmd);
    Misc::moonwalk(cmd);
    Misc::fastPlant(*engineInterfaces->engineTrace, *interfaces, cmd);

    if (!(cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2))) {
        Misc::chokePackets(*engineInterfaces->engine, sendPacket);
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

void GlobalContext::doPostScreenEffectsHook(void* param)
{
    if (engineInterfaces->engine->isInGame()) {
        Visuals::thirdperson(*memory);
        Visuals::inverseRagdollGravity(*interfaces);
        Visuals::reduceFlashEffect();
        Visuals::updateBrightness(*interfaces);
        Visuals::remove3dSky(*interfaces);
        Glow::render(*engineInterfaces, *clientInterfaces, *interfaces, *memory);
    }
    hooks->clientMode.callOriginal<void, WIN32_LINUX(44, 45)>(param);
}

float GlobalContext::getViewModelFovHook()
{
    float additionalFov = Visuals::viewModelFov();
    if (localPlayer) {
        if (const auto activeWeapon = localPlayer->getActiveWeapon(); activeWeapon && activeWeapon->getClientClass()->classId == ClassId::Tablet)
            additionalFov = 0.0f;
    }

    return hooks->clientMode.callOriginal<float, WIN32_LINUX(35, 36)>() + additionalFov;
}

void GlobalContext::drawModelExecuteHook(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld)
{
    if (interfaces->studioRender->isForcedMaterialOverride())
        return hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);

    if (Visuals::removeHands(info.model->name) || Visuals::removeSleeves(info.model->name) || Visuals::removeWeapons(info.model->name))
        return;

    if (static Chams chams; !chams.render(*engineInterfaces->engine, *clientInterfaces, *interfaces, *memory, *config, ctx, state, info, customBoneToWorld))
        hooks->modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);

    interfaces->studioRender->forcedMaterialOverride(nullptr);
}

bool GlobalContext::svCheatsGetBoolHook(void* _this, std::uintptr_t returnAddress)
{
    if (returnAddress == memory->cameraThink && Visuals::isThirdpersonOn())
        return true;

    return hooks->svCheats.getOriginal<bool, WIN32_LINUX(13, 16)>()(_this);
}

void GlobalContext::frameStageNotifyHook(csgo::FrameStage stage)
{
    [[maybe_unused]] static auto backtrackInit = (Backtrack::init(*interfaces), false);
    if (engineInterfaces->engine->isConnected() && !engineInterfaces->engine->isInGame())
        Misc::changeName(*engineInterfaces->engine, *interfaces, *memory, true, nullptr, 0.0f);

    if (stage == csgo::FrameStage::START)
        GameData::update(*clientInterfaces, *engineInterfaces, *interfaces, *memory);

    if (stage == csgo::FrameStage::RENDER_START) {
        Misc::preserveKillfeed(*memory);
        Misc::disablePanoramablur(*interfaces);
        Visuals::colorWorld(*interfaces, *memory);
        Misc::updateEventListeners(*clientInterfaces, *engineInterfaces, *interfaces, *memory);
        Visuals::updateEventListeners(*engineInterfaces, *clientInterfaces, *interfaces, *memory);
    }
    if (engineInterfaces->engine->isInGame()) {
        Visuals::skybox(*interfaces, *memory, stage);
        Visuals::removeBlur(*interfaces, stage);
        Misc::oppositeHandKnife(*interfaces, stage);
        Visuals::removeGrass(*engineInterfaces->engine, *interfaces, stage);
        Visuals::modifySmoke(*interfaces, stage);
        Visuals::disablePostProcessing(*memory, stage);
        Visuals::removeVisualRecoil(stage);
        Visuals::applyZoom(stage);
        Misc::fixAnimationLOD(*engineInterfaces->engine, *clientInterfaces, *memory, stage);
        Backtrack::update(*engineInterfaces, *clientInterfaces, *interfaces, *memory, stage);
    }
    inventory_changer::InventoryChanger::instance(*interfaces, *memory).run(*engineInterfaces, *clientInterfaces, *interfaces, *memory, stage);

    hooks->client.callOriginal<void, 37>(stage);
}

bool GlobalContext::shouldDrawFogHook(std::uintptr_t returnAddress)
{
#ifdef _WIN32
    if constexpr (std::is_same_v<HookType, MinHook>) {
        if (returnAddress != memory->shouldDrawFogReturnAddress)
            return hooks->clientMode.callOriginal<bool, 17>();
    }
#endif

    return !Visuals::shouldRemoveFog();
}

bool GlobalContext::shouldDrawViewModelHook()
{
    if (Visuals::isZoomOn() && localPlayer && localPlayer->fov() < 45 && localPlayer->fovStart() < 45)
        return false;
    return hooks->clientMode.callOriginal<bool, WIN32_LINUX(27, 28)>();
}

void GlobalContext::lockCursorHook()
{
    if (gui->isOpen())
        return interfaces->surface->unlockCursor();
    return hooks->surface.callOriginal<void, 67>();
}

void GlobalContext::setDrawColorHook(int r, int g, int b, int a, std::uintptr_t returnAddress)
{
    if (Visuals::shouldRemoveScopeOverlay() && (returnAddress == memory->scopeDust || returnAddress == memory->scopeArc))
        a = 0;
    hooks->surface.callOriginal<void, WIN32_LINUX(15, 14)>(r, g, b, a);
}

void GlobalContext::overrideViewHook(ViewSetup* setup)
{
    if (localPlayer && !localPlayer->isScoped())
        setup->fov += Visuals::fov();
    setup->farZ += Visuals::farZ() * 10;
    hooks->clientMode.callOriginal<void, WIN32_LINUX(18, 19)>(setup);
}

int GlobalContext::dispatchSoundHook(SoundInfo& soundInfo)
{
    if (const char* soundName = interfaces->soundEmitter->getSoundName(soundInfo.soundIndex)) {
        Sound::modulateSound(*clientInterfaces, *memory, soundName, soundInfo.entityIndex, soundInfo.volume);
        soundInfo.volume = std::clamp(soundInfo.volume, 0.0f, 1.0f);
    }
    return hooks->originalDispatchSound(soundInfo);
}

void GlobalContext::render2dEffectsPreHudHook(void* viewSetup)
{
    Visuals::applyScreenEffects(*engineInterfaces->engine, *interfaces, *memory);
    Visuals::hitEffect(*engineInterfaces->engine, *interfaces, *memory);
    hooks->viewRender.callOriginal<void, WIN32_LINUX(39, 40)>(viewSetup);
}

const DemoPlaybackParameters* GlobalContext::getDemoPlaybackParametersHook(std::uintptr_t returnAddress)
{
    const auto params = hooks->engine.callOriginal<const DemoPlaybackParameters*, WIN32_LINUX(218, 219)>();

    if (params && Misc::shouldRevealSuspect() && returnAddress != memory->demoFileEndReached) {
        static DemoPlaybackParameters customParams;
        customParams = *params;
        customParams.anonymousPlayerIdentity = false;
        return &customParams;
    }

    return params;
}

bool GlobalContext::dispatchUserMessageHook(csgo::UserMessageType type, int passthroughFlags, int size, const void* data)
{
    if (type == csgo::UserMessageType::Text)
        inventory_changer::InventoryChanger::instance(*interfaces, *memory).onUserTextMsg(*memory, data, size);
    else if (type == csgo::UserMessageType::VoteStart)
        Misc::onVoteStart(*globalContext->clientInterfaces, *interfaces, *memory, data, size);
    else if (type == csgo::UserMessageType::VotePass)
        Misc::onVotePass(*memory);
    else if (type == csgo::UserMessageType::VoteFailed)
        Misc::onVoteFailed(*memory);

    return hooks->client.callOriginal<bool, 38>(type, passthroughFlags, size, data);
}

#ifdef _WIN32
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT GlobalContext::wndProcHook(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (state == GlobalContext::State::Initialized) {
        ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam);
        interfaces->inputSystem->enableInput(!gui->isOpen());
    } else if (state == GlobalContext::State::NotInitialized) {
        state = GlobalContext::State::Initializing;

        clientInterfaces.emplace(InterfaceFinder{ DynamicLibraryView<windows_platform::DynamicLibraryWrapper>{ windows_platform::DynamicLibraryWrapper{}, CLIENT_DLL }, retSpoofGadgets.jmpEbxInClient });
        engineInterfaces.emplace(InterfaceFinder{ DynamicLibraryView<windows_platform::DynamicLibraryWrapper>{ windows_platform::DynamicLibraryWrapper{}, ENGINE_DLL }, retSpoofGadgets.jmpEbxInClient });
        interfaces.emplace(Interfaces{});

        memory.emplace(Memory{ *clientInterfaces->client, retSpoofGadgets });

        Netvars::init(*clientInterfaces->client);
        gameEventListener.emplace(*memory, *clientInterfaces, *engineInterfaces, *interfaces);

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(window);
        config.emplace(Config{ *interfaces, *memory });
        gui.emplace(GUI{});
        hooks->install(*interfaces, *memory);

        state = GlobalContext::State::Initialized;
    }

    return CallWindowProcW(hooks->originalWndProc, window, msg, wParam, lParam);
}

HRESULT GlobalContext::presentHook(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion)
{
    [[maybe_unused]] static bool imguiInit{ ImGui_ImplDX9_Init(device) };

    if (config->loadScheduledFonts())
        ImGui_ImplDX9_DestroyFontsTexture();

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    renderFrame();

    if (device->BeginScene() == D3D_OK) {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    //
    GameData::clearUnusedAvatars();
    InventoryChanger::clearUnusedItemIconTextures();
    //

    return hooks->originalPresent(device, src, dest, windowOverride, dirtyRegion);
}

#else
int GlobalContext::pollEventHook(SDL_Event* event)
{
    const auto result = hooks->pollEvent(event);

    if (state == GlobalContext::State::Initialized) {
        if (result && ImGui_ImplSDL2_ProcessEvent(event) && gui->isOpen())
            event->type = 0;
    } else if (state == GlobalContext::State::NotInitialized) {
        state = GlobalContext::State::Initializing;

        const linux_platform::SharedObject clientSo{ linux_platform::DynamicLibraryWrapper{}, CLIENT_DLL };
        clientInterfaces.emplace(InterfaceFinder{ clientSo.getView(), retSpoofGadgets.jmpEbxInClient });
        const linux_platform::SharedObject engineSo{ linux_platform::DynamicLibraryWrapper{}, ENGINE_DLL };
        engineInterfaces.emplace(InterfaceFinder{ engineSo.getView(), retSpoofGadgets.jmpEbxInClient });

        interfaces.emplace(Interfaces{});
        memory.emplace(Memory{ *clientInterfaces->client, retSpoofGadgets });

        Netvars::init(*clientInterfaces->client);
        gameEventListener.emplace(*memory, *clientInterfaces, *engineInterfaces, *interfaces);

        ImGui::CreateContext();
        config.emplace(Config{ *interfaces, *memory });

        gui.emplace(GUI{});
        hooks->install(*interfaces, *memory);

        state = GlobalContext::State::Initialized;
    }
    
    return result;
}

void GlobalContext::swapWindowHook(SDL_Window* window)
{
    [[maybe_unused]] static const auto _ = ImGui_ImplSDL2_InitForOpenGL(window, nullptr);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);

    renderFrame();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    GameData::clearUnusedAvatars();
    InventoryChanger::clearUnusedItemIconTextures();

    hooks->swapWindow(window);
}

#endif

void GlobalContext::viewModelSequenceNetvarHook(recvProxyData& data, void* outStruct, void* arg3)
{
    const auto viewModel = reinterpret_cast<Entity*>(outStruct);

    if (localPlayer && clientInterfaces->entityList->getEntityFromHandle(viewModel->owner()) == localPlayer.get()) {
        if (const auto weapon = clientInterfaces->entityList->getEntityFromHandle(viewModel->weapon())) {
            if (Visuals::isDeagleSpinnerOn() && weapon->getClientClass()->classId == ClassId::Deagle && data.value._int == 7)
                data.value._int = 8;

            inventory_changer::InventoryChanger::instance(*interfaces, *memory).fixKnifeAnimation(weapon, data.value._int);
        }
    }

    proxyHooks.viewModelSequence.originalProxy(data, outStruct, arg3);
}

void GlobalContext::renderFrame()
{
    ImGui::NewFrame();

    if (const auto& displaySize = ImGui::GetIO().DisplaySize; displaySize.x > 0.0f && displaySize.y > 0.0f) {
        StreamProofESP::render(*memory, *config);
        Misc::purchaseList(*engineInterfaces->engine, *clientInterfaces, *interfaces, *memory);
        Misc::noscopeCrosshair(*memory, ImGui::GetBackgroundDrawList());
        Misc::recoilCrosshair(*memory, ImGui::GetBackgroundDrawList());
        Misc::drawOffscreenEnemies(*engineInterfaces->engine, *memory, ImGui::GetBackgroundDrawList());
        Misc::drawBombTimer(*memory);
        Misc::spectatorList();
        Visuals::hitMarker(*engineInterfaces->engine, *interfaces, *memory, nullptr, ImGui::GetBackgroundDrawList());
        Visuals::drawMolotovHull(*memory, ImGui::GetBackgroundDrawList());
        Misc::watermark(*memory);

        Aimbot::updateInput(*config);
        Visuals::updateInput();
        StreamProofESP::updateInput(*config);
        Misc::updateInput();
        Triggerbot::updateInput(*config);
        Chams::updateInput(*config);
        Glow::updateInput();

        gui->handleToggle(*interfaces);

        if (gui->isOpen())
            gui->render(*engineInterfaces->engine, *interfaces, *memory, *config);
    }

    ImGui::EndFrame();
    ImGui::Render();
}
