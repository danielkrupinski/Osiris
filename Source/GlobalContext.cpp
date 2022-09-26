#include "GlobalContext.h"

#ifdef _WIN32
#include <imgui/imgui_impl_win32.h>
#else
#include <imgui/imgui_impl_sdl.h>
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
#include "Hacks/Triggerbot.h"
#include "Hacks/Visuals.h"
#include "SDK/ClientClass.h"
#include "SDK/Constants/ClassId.h"
#include "SDK/Constants/FrameStage.h"
#include "SDK/Engine.h"
#include "SDK/Entity.h"
#include "SDK/GlobalVars.h"
#include "SDK/InputSystem.h"
#include "SDK/LocalPlayer.h"
#include "SDK/ModelRender.h"
#include "SDK/StudioRender.h"
#include "SDK/UserCmd.h"

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
    Misc::prepareRevolver(*interfaces, *memory, cmd);
    Visuals::removeShadows(*interfaces);
    Misc::runReportbot(*interfaces, *memory);
    Misc::bunnyHop(cmd);
    Misc::autoStrafe(cmd);
    Misc::removeCrouchCooldown(cmd);
    Misc::autoPistol(*memory, cmd);
    Misc::autoReload(cmd);
    Misc::updateClanTag(*memory);
    Misc::fakeBan(*interfaces, *memory);
    Misc::stealNames(*interfaces, *memory);
    Misc::revealRanks(*interfaces, cmd);
    Misc::quickReload(*interfaces, cmd);
    Misc::fixTabletSignal();
    Misc::slowwalk(cmd);

    EnginePrediction::run(*interfaces, *memory, cmd);

    Aimbot::run(*interfaces, *config, *memory, cmd);
    Triggerbot::run(*interfaces, *memory, *config, cmd);
    Backtrack::run(*interfaces, *memory, cmd);
    Misc::edgejump(cmd);
    Misc::moonwalk(cmd);
    Misc::fastPlant(*interfaces, cmd);

    if (!(cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2))) {
        Misc::chokePackets(*interfaces, sendPacket);
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
    if (interfaces->engine->isInGame()) {
        Visuals::thirdperson(*memory);
        Visuals::inverseRagdollGravity(*interfaces);
        Visuals::reduceFlashEffect();
        Visuals::updateBrightness(*interfaces);
        Visuals::remove3dSky(*interfaces);
        Glow::render(*interfaces, *memory);
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

    if (static Chams chams; !chams.render(*interfaces, *memory, *config, ctx, state, info, customBoneToWorld))
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
    if (interfaces->engine->isConnected() && !interfaces->engine->isInGame())
        Misc::changeName(*interfaces, *memory, true, nullptr, 0.0f);

    if (stage == csgo::FrameStage::START)
        GameData::update(*interfaces, *memory);

    if (stage == csgo::FrameStage::RENDER_START) {
        Misc::preserveKillfeed(*memory);
        Misc::disablePanoramablur(*interfaces);
        Visuals::colorWorld(*interfaces, *memory);
        Misc::updateEventListeners(*interfaces, *memory);
        Visuals::updateEventListeners(*interfaces, *memory);
    }
    if (interfaces->engine->isInGame()) {
        Visuals::skybox(*interfaces, *memory, stage);
        Visuals::removeBlur(*interfaces, stage);
        Misc::oppositeHandKnife(*interfaces, stage);
        Visuals::removeGrass(*interfaces, stage);
        Visuals::modifySmoke(*interfaces, stage);
        Visuals::disablePostProcessing(*memory, stage);
        Visuals::removeVisualRecoil(stage);
        Visuals::applyZoom(stage);
        Misc::fixAnimationLOD(*interfaces, *memory, stage);
        Backtrack::update(*interfaces, *memory, stage);
    }
    inventory_changer::InventoryChanger::instance(*interfaces, *memory).run(*interfaces, *memory, stage);

    hooks->client.callOriginal<void, 37>(stage);
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

        interfaces.emplace(Interfaces{});
        memory.emplace(Memory{ *interfaces->client });

        Netvars::init(*interfaces);
        gameEventListener.emplace(*memory, *interfaces);

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(window);
        config.emplace(Config{ *interfaces, *memory });
        gui.emplace(GUI{});
        hooks->install(*interfaces, *memory);

        state = GlobalContext::State::Initialized;
    }

    return CallWindowProcW(hooks->originalWndProc, window, msg, wParam, lParam);
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

        Netvars::init(*interfaces);
        gameEventListener.emplace(*memory, *interfaces);

        ImGui::CreateContext();
        config.emplace(Config{ *interfaces, *memory });

        gui.emplace(GUI{});
        hooks->install(*interfaces, *memory);

        state = GlobalContext::State::Initialized;
    }
    
    return result;
}
#endif
