#pragma once

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32() || IS_WIN64()
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "Hooks/WindowProcedureHook.h"
#endif

#include "EventListener.h"
#include "GlobalContext.h"

#include <Platform/Macros/CallStack.h>

#include "Hooks/BspQueryHooks.h"
#include "Hooks/ClientHooks.h"
#include "Hooks/ClientModeHooks.h"
#include "Hooks/ClientStateHooks.h"
#include "Hooks/CSPlayerInventoryHooks.h"
#include "Hooks/EngineHooks.h"
#include "Hooks/EngineSoundHooks.h"
#include "Hooks/InventoryManagerHooks.h"
#include "Hooks/KeyValuesSystemHooks.h"
#include "Hooks/ModelRenderHooks.h"
#include "Hooks/PanoramaMarshallHelperHooks.h"
#include "Hooks/SurfaceHooks.h"
#include "Hooks/SvCheatsHooks.h"
#include "Hooks/ViewRenderHooks.h"

#include "CSGO/Constants/GameEventNames.h"
#include "CSGO/Constants/UserMessages.h"
#include "CSGO/ClientClass.h"
#include "CSGO/GameEvent.h"
#include "CSGO/GlobalVars.h"
#include "CSGO/LocalPlayer.h"
#include "CSGO/ModelRender.h"
#include "CSGO/SoundInfo.h"
#include "CSGO/UserCmd.h"
#include "CSGO/ViewSetup.h"
#include "GameData.h"
#include "GUI.h"
#include "Hacks/EnginePrediction.h"
#include "Hacks/Sound.h"
#include "Hacks/Triggerbot.h"
#include "Hooks.h"

void fireGameEvent(csgo::GameEventPOD* eventPointer)
{
    const auto event = csgo::GameEvent::from(retSpoofGadgets->client, eventPointer);

    switch (fnv::hashRuntime(event.getName())) {
    case fnv::hash(csgo::round_start):
        GameData::clearProjectileList();
        globalContext->features->misc.preserveKillfeed(true);
        [[fallthrough]];
    case fnv::hash(csgo::round_freeze_end):
        globalContext->features->misc.purchaseList(&event);
        break;
    case fnv::hash(csgo::player_death):
        globalContext->features->inventoryChanger.updateStatTrak(event);
        globalContext->features->inventoryChanger.overrideHudIcon(*globalContext->memory, event);
        globalContext->features->misc.killMessage(event);
        globalContext->features->misc.killSound(event);
        break;
    case fnv::hash(csgo::player_hurt):
        globalContext->features->misc.playHitSound(event);
        globalContext->features->visuals.hitEffect(&event);
        globalContext->features->visuals.hitMarker(&event);
        break;
    case fnv::hash(csgo::vote_cast):
        globalContext->features->misc.voteRevealer(event);
        break;
    case fnv::hash(csgo::round_mvp):
        globalContext->features->inventoryChanger.onRoundMVP(event);
        break;
    case fnv::hash(csgo::item_purchase):
        globalContext->features->misc.purchaseList(&event);
        break;
    case fnv::hash(csgo::bullet_impact):
        globalContext->features->visuals.bulletTracer(event);
        break;
    }
}

void DefaultEventListener::fireGameEvent(csgo::GameEventPOD* eventPointer)
{
    ::fireGameEvent(eventPointer);
}

void EventListener::fireGameEvent(csgo::GameEventPOD* eventPointer)
{
    ::fireGameEvent(eventPointer);
}

void CDECL_CONV viewModelSequence(csgo::recvProxyData* data, void* outStruct, void* arg3) noexcept
{
    const auto viewModel = csgo::Entity::from(retSpoofGadgets->client, static_cast<csgo::EntityPOD*>(outStruct));

    if (localPlayer && ClientInterfaces{ retSpoofGadgets->client, *globalContext->clientInterfaces }.getEntityList().getEntityFromHandle(viewModel.owner()) == localPlayer.get().getPOD()) {
        if (const auto weapon = csgo::Entity::from(retSpoofGadgets->client, ClientInterfaces{ retSpoofGadgets->client, *globalContext->clientInterfaces }.getEntityList().getEntityFromHandle(viewModel.weapon())); weapon.getPOD() != nullptr) {
            if (globalContext->features->visuals.isDeagleSpinnerOn() && weapon.getNetworkable().getClientClass()->classId == ClassId::Deagle && data->value._int == 7)
                data->value._int = 8;

            globalContext->features->inventoryChanger.fixKnifeAnimation(weapon, data->value._int, *globalContext->randomGenerator);
        }
    }

    proxyHooks.viewModelSequence.originalProxy(data, outStruct, arg3);
}

void CDECL_CONV spottedHook(csgo::recvProxyData* data, void* outStruct, void* arg3) noexcept
{
    if (globalContext->features->misc.isRadarHackOn()) {
        data->value._int = 1;

        if (localPlayer) {
            const auto entity = csgo::Entity::from(retSpoofGadgets->client, static_cast<csgo::EntityPOD*>(outStruct));
            if (const auto index = localPlayer.get().getNetworkable().index(); index > 0 && index <= 32)
                entity.spottedByMask() |= 1 << (index - 1);
        }
    }

    proxyHooks.spotted.originalProxy(data, outStruct, arg3);
}

#if IS_WIN32() || IS_WIN64()

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall WindowProcedureHook::wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (globalContext->state == GlobalContextState::Initialized) {
        ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam);
        globalContext->getOtherInterfaces().getInputSystem().enableInput(!gui->isOpen());
    } else if (globalContext->state == GlobalContextState::NotInitialized) {
        globalContext->state = GlobalContextState::Initializing;
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(window);
        globalContext->initialize();
        globalContext->state = GlobalContextState::Initialized;
    }

    return CallWindowProcW(globalContext->windowProcedureHook->originalWndProc, window, msg, wParam, lParam);
}

HRESULT __stdcall reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    globalContext->features->inventoryChanger.clearItemIconTextures();
    GameData::clearTextures();
    return globalContext->hooks->originalReset(device, params);
}

HRESULT __stdcall present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
{
    [[maybe_unused]] static bool imguiInit{ ImGui_ImplDX9_Init(device) };

    if (globalContext->config->loadScheduledFonts())
        ImGui_ImplDX9_DestroyFontsTexture();

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    globalContext->renderFrame();

    if (device->BeginScene() == D3D_OK) {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    //
    GameData::clearUnusedAvatars();
    globalContext->features->inventoryChanger.clearUnusedItemIconTextures();
    //

    return globalContext->hooks->originalPresent(device, src, dest, windowOverride, dirtyRegion);
}

#elif IS_LINUX()

int pollEvent(SDL_Event* event) noexcept
{
    return globalContext->pollEventHook(event);
}

void swapWindow(SDL_Window* window) noexcept
{
    globalContext->swapWindowHook(window);
}

#endif

bool FASTCALL_CONV ClientModeHooks::createMove(FASTCALL_THIS(csgo::ClientMode* thisptr), float inputSampleTime, csgo::UserCmd* cmd) noexcept
{
    auto result = globalContext->hooks->clientModeHooks.getOriginalCreateMove()(thisptr, inputSampleTime, cmd);

    if (!cmd->commandNumber)
        return result;

    static auto previousViewAngles{ cmd->viewangles };
    const auto currentViewAngles{ cmd->viewangles };

    globalContext->memory->globalVars->serverTime(cmd);
    auto& features = globalContext->features;
    features->misc.nadePredict();
    features->misc.antiAfkKick(cmd);
    features->misc.fastStop(cmd);
    features->misc.prepareRevolver(cmd);
    features->visuals.removeShadows();
    features->misc.runReportbot();
    features->misc.bunnyHop(cmd);
    features->misc.autoStrafe(cmd);
    features->misc.removeCrouchCooldown(cmd);
    features->misc.autoPistol(cmd);
    features->misc.autoReload(cmd);
    features->misc.updateClanTag();
    features->misc.fakeBan();
    features->misc.stealNames();
    features->misc.revealRanks(cmd);
    features->misc.quickReload(cmd);
    features->misc.fixTabletSignal();
    features->misc.slowwalk(cmd);

    EnginePrediction::run(ClientInterfaces{ retSpoofGadgets->client, *globalContext->clientInterfaces }, *globalContext->memory, cmd);

    features->aimbot.run(features->misc, globalContext->getEngineInterfaces(), ClientInterfaces{ retSpoofGadgets->client, *globalContext->clientInterfaces }, globalContext->getOtherInterfaces(), *globalContext->config, *globalContext->memory, cmd);
    Triggerbot::run(globalContext->getEngineInterfaces().engineTrace(), globalContext->getOtherInterfaces(), *globalContext->memory, *globalContext->config, cmd);
    features->backtrack.run(ClientInterfaces{ retSpoofGadgets->client, *globalContext->clientInterfaces }, globalContext->getEngineInterfaces(), globalContext->getOtherInterfaces(), *globalContext->memory, cmd);
    features->misc.edgejump(cmd);
    features->misc.moonwalk(cmd);
    features->misc.fastPlant(cmd);

    auto viewAnglesDelta{ cmd->viewangles - previousViewAngles };
    viewAnglesDelta.normalize();
    viewAnglesDelta.x = std::clamp(viewAnglesDelta.x, -features->misc.maxAngleDelta(), features->misc.maxAngleDelta());
    viewAnglesDelta.y = std::clamp(viewAnglesDelta.y, -features->misc.maxAngleDelta(), features->misc.maxAngleDelta());

    cmd->viewangles = previousViewAngles + viewAnglesDelta;

    cmd->viewangles.normalize();
    features->misc.fixMovement(cmd, currentViewAngles.y);

    cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
    cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
    cmd->viewangles.z = 0.0f;
    cmd->forwardmove = std::clamp(cmd->forwardmove, -450.0f, 450.0f);
    cmd->sidemove = std::clamp(cmd->sidemove, -450.0f, 450.0f);

    previousViewAngles = cmd->viewangles;

    cmd->viewanglesBackup = cmd->viewangles;
    cmd->buttonsBackup = cmd->buttons;

    return false;
}

void FASTCALL_CONV ClientModeHooks::doPostScreenEffects(FASTCALL_THIS(csgo::ClientMode* thisptr), void* param) noexcept
{
    auto& features = globalContext->features;
    if (globalContext->getEngineInterfaces().getEngine().isInGame()) {
        features->visuals.thirdperson();
        features->visuals.inverseRagdollGravity();
        features->visuals.reduceFlashEffect();
        features->visuals.updateBrightness();
        features->visuals.remove3dSky();
        features->glow.render(globalContext->getEngineInterfaces(), ClientInterfaces{ retSpoofGadgets->client, *globalContext->clientInterfaces }, *globalContext->memory);
    }
    globalContext->hooks->clientModeHooks.getOriginalDoPostScreenEffects()(thisptr, param);
}

float FASTCALL_CONV ClientModeHooks::getViewModelFov(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept
{
    float additionalFov = globalContext->features->visuals.viewModelFov();
    if (localPlayer) {
        if (const auto activeWeapon = csgo::Entity::from(retSpoofGadgets->client, localPlayer.get().getActiveWeapon()); activeWeapon.getPOD() != nullptr && activeWeapon.getNetworkable().getClientClass()->classId == ClassId::Tablet)
            additionalFov = 0.0f;
    }

    return globalContext->hooks->clientModeHooks.getOriginalGetViewModelFov()(thisptr) + additionalFov;
}

void FASTCALL_CONV ModelRenderHooks::drawModelExecute(FASTCALL_THIS(csgo::ModelRenderPOD* thisptr), void* ctx, void* state, const csgo::ModelRenderInfo& info, csgo::matrix3x4* customBoneToWorld) noexcept
{
    if (globalContext->getOtherInterfaces().getStudioRender().isForcedMaterialOverride())
        return globalContext->hooks->modelRenderHooks.getOriginalDrawModelExecute()(thisptr, ctx, state, &info, customBoneToWorld);

    auto& features = globalContext->features;
    if (features->visuals.removeHands(info.model->name) || features->visuals.removeSleeves(info.model->name) || features->visuals.removeWeapons(info.model->name))
        return;

    if (!features->chams.render(features->backtrack, ctx, state, info, customBoneToWorld))
        globalContext->hooks->modelRenderHooks.getOriginalDrawModelExecute()(thisptr, ctx, state, &info, customBoneToWorld);

    globalContext->getOtherInterfaces().getStudioRender().forcedMaterialOverride(nullptr);
}

int FASTCALL_CONV SvCheatsHooks::getInt(csgo::ConVarPOD* thisptr) noexcept
{
    const auto original = globalContext->hooks->svCheatsHooks.getOriginalSvCheatsGetInt()(thisptr);
    if (globalContext->features->visuals.svCheatsGetBoolHook(RETURN_ADDRESS()))
        return 1;
    return original;
}

void FASTCALL_CONV ClientHooks::frameStageNotify(FASTCALL_THIS(csgo::ClientPOD* thisptr), csgo::FrameStage stage) noexcept
{
    auto& features = globalContext->features;
    if (globalContext->getEngineInterfaces().getEngine().isConnected() && !globalContext->getEngineInterfaces().getEngine().isInGame())
        features->misc.changeName(true, nullptr, 0.0f);

    if (stage == csgo::FrameStage::START)
        GameData::update(ClientInterfaces{ retSpoofGadgets->client, *globalContext->clientInterfaces }, globalContext->getEngineInterfaces(), globalContext->getOtherInterfaces(), *globalContext->memory);

    if (stage == csgo::FrameStage::RENDER_START) {
        features->misc.preserveKillfeed();
        features->misc.disablePanoramablur();
        features->visuals.colorWorld();
        features->misc.updateEventListeners();
        features->visuals.updateEventListeners();
    }
    if (globalContext->getEngineInterfaces().getEngine().isInGame()) {
        features->visuals.skybox(stage);
        features->visuals.removeBlur(stage);
        features->misc.oppositeHandKnife(stage);
        features->visuals.removeGrass(stage);
        features->visuals.modifySmoke(stage);
        features->visuals.disablePostProcessing(stage);
        features->visuals.removeVisualRecoil(stage);
        features->visuals.applyZoom(stage);
        features->misc.fixAnimationLOD(stage);
        features->backtrack.update(globalContext->getEngineInterfaces(), ClientInterfaces{ retSpoofGadgets->client, *globalContext->clientInterfaces }, globalContext->getOtherInterfaces(), *globalContext->memory, stage);
    }
    features->inventoryChanger.run(*globalContext->memory, stage);

    globalContext->hooks->clientHooks.getOriginalFrameStageNotify()(thisptr, stage);
}

int FASTCALL_CONV EngineSoundHooks::emitSound(FASTCALL_THIS(csgo::EngineSoundPOD* thisptr), void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const csgo::Vector& origin, const csgo::Vector& direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams) noexcept
{
    globalContext->features->sound.modulateSound(soundEntry, entityIndex, volume);
    globalContext->features->misc.autoAccept(soundEntry);

    volume = std::clamp(volume, 0.0f, 1.0f);
    return globalContext->hooks->engineSoundHooks.getOriginalEmitSound()(thisptr, filter, entityIndex, channel, soundEntry, soundEntryHash, sample, volume, seed, soundLevel, flags, pitch, &origin, &direction, utlVecOrigins, updatePositions, soundtime, speakerentity, soundParams);
}

bool FASTCALL_CONV ClientModeHooks::shouldDrawFog(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept
{
#if IS_WIN32()
    if constexpr (std::is_same_v<HookType, MinHook>) {
        if (RETURN_ADDRESS() != globalContext->memory->shouldDrawFogReturnAddress)
            return globalContext->hooks->clientModeHooks.getOriginalShouldDrawFog()(thisptr);
    }
#endif

    return !globalContext->features->visuals.shouldRemoveFog();
}

bool FASTCALL_CONV ClientModeHooks::shouldDrawViewModel(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept
{
    if (globalContext->features->visuals.isZoomOn() && localPlayer && localPlayer.get().fov() < 45 && localPlayer.get().fovStart() < 45)
        return false;
    return globalContext->hooks->clientModeHooks.getOriginalShouldDrawViewModel()(thisptr);
}

#if IS_WIN32()
void FASTCALL_CONV SurfaceHooks::lockCursor(FASTCALL_THIS(csgo::SurfacePOD* thisptr)) noexcept
{
    if (gui->isOpen())
        return globalContext->getOtherInterfaces().getSurface().unlockCursor();
    return globalContext->hooks->surfaceHooks.getOriginalLockCursor()(thisptr);
}
#endif

void FASTCALL_CONV SurfaceHooks::setDrawColor(FASTCALL_THIS(csgo::SurfacePOD* thisptr), int r, int g, int b, int a) noexcept
{
    globalContext->features->visuals.setDrawColorHook(RETURN_ADDRESS(), a);
    globalContext->hooks->surfaceHooks.getOriginalSetDrawColor()(thisptr, r, g, b, a);
}

void FASTCALL_CONV ClientModeHooks::overrideView(FASTCALL_THIS(csgo::ClientMode* thisptr), csgo::ViewSetup* setup) noexcept
{
    if (localPlayer && !localPlayer.get().isScoped())
        setup->fov += globalContext->features->visuals.fov();
    setup->farZ += globalContext->features->visuals.farZ() * 10;
    globalContext->hooks->clientModeHooks.getOriginalOverrideView()(thisptr, setup);
}

int FASTCALL_CONV BspQueryHooks::listLeavesInBox(FASTCALL_THIS(void* thisptr), const csgo::Vector& mins, const csgo::Vector& maxs, unsigned short* list, int listMax) noexcept
{
    if (const auto newVectors = globalContext->features->misc.listLeavesInBoxHook(RETURN_ADDRESS(), FRAME_ADDRESS()))
        return globalContext->hooks->bspQueryHooks.getOriginalListLeavesInBox()(thisptr, &newVectors->first, &newVectors->second, list, listMax);
    return globalContext->hooks->bspQueryHooks.getOriginalListLeavesInBox()(thisptr, &mins, &maxs, list, listMax);
}

void FASTCALL_CONV ViewRenderHooks::render2dEffectsPreHud(FASTCALL_THIS(csgo::ViewRender* thisptr), void* viewSetup) noexcept
{
    globalContext->features->visuals.applyScreenEffects();
    globalContext->features->visuals.hitEffect();
    globalContext->hooks->viewRenderHooks.getOriginalRender2dEffectsPreHud()(thisptr, viewSetup);
}

const csgo::DemoPlaybackParameters* FASTCALL_CONV EngineHooks::getDemoPlaybackParameters(FASTCALL_THIS(csgo::EnginePOD* thisptr)) noexcept
{
    const auto params = globalContext->hooks->engineHooks.getOriginalGetDemoPlaybackParameters()(thisptr);

    if (params)
        return globalContext->features->misc.getDemoPlaybackParametersHook(RETURN_ADDRESS(), *params);

    return params;
}

bool FASTCALL_CONV EngineHooks::isPlayingDemo(FASTCALL_THIS(csgo::EnginePOD* thisptr)) noexcept
{
    const auto result = globalContext->hooks->engineHooks.getOriginalIsPlayingDemo()(thisptr);

    if (globalContext->features->misc.isPlayingDemoHook(RETURN_ADDRESS(), FRAME_ADDRESS()))
        return true;

    return result;
}

void FASTCALL_CONV ClientModeHooks::updateColorCorrectionWeights(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept
{
    globalContext->hooks->clientModeHooks.getOriginalUpdateColorCorrectionWeights()(thisptr);
    globalContext->features->visuals.updateColorCorrectionWeightsHook();
}

float FASTCALL_CONV EngineHooks::getScreenAspectRatio(FASTCALL_THIS(csgo::EnginePOD* thisptr), int width, int height) noexcept
{
    if (globalContext->features->misc.aspectRatio() != 0.0f)
        return globalContext->features->misc.aspectRatio();
    return globalContext->hooks->engineHooks.getOriginalGetScreenAspectRatio()(thisptr, width, height);
}

void FASTCALL_CONV ViewRenderHooks::renderSmokeOverlay(FASTCALL_THIS(csgo::ViewRender* thisptr), bool preViewModel) noexcept
{
    if (!globalContext->features->visuals.renderSmokeOverlayHook())
        globalContext->hooks->viewRenderHooks.getOriginalRenderSmokeOverlay()(thisptr, preViewModel);
}

double FASTCALL_CONV PanoramaMarshallHelperHooks::getArgAsNumber(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params, int index) noexcept
{
    const auto result = globalContext->hooks->panoramaMarshallHelperHooks.getOriginalGetArgAsNumber()(thisptr, params, index);
    globalContext->features->inventoryChanger.getArgAsNumberHook(static_cast<int>(result), RETURN_ADDRESS());
    return result;
}

const char* FASTCALL_CONV PanoramaMarshallHelperHooks::getArgAsString(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params, int index) noexcept
{
    const auto result = globalContext->hooks->panoramaMarshallHelperHooks.getOriginalGetArgAsString()(thisptr, params, index);

    if (result)
        globalContext->features->inventoryChanger.getArgAsStringHook(globalContext->hooks->panoramaMarshallHelperHooks, *globalContext->memory, result, RETURN_ADDRESS(), params);

    return result;
}

void FASTCALL_CONV PanoramaMarshallHelperHooks::setResultInt(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params, int result) noexcept
{
    result = globalContext->features->inventoryChanger.setResultIntHook(RETURN_ADDRESS(), params, result);
    globalContext->hooks->panoramaMarshallHelperHooks.getOriginalSetResultInt()(thisptr, params, result);
}

unsigned FASTCALL_CONV PanoramaMarshallHelperHooks::getNumArgs(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params) noexcept
{
    const auto result = globalContext->hooks->panoramaMarshallHelperHooks.getOriginalGetNumArgs()(globalContext->memory->panoramaMarshallHelper, params);
    globalContext->features->inventoryChanger.getNumArgsHook(globalContext->hooks->panoramaMarshallHelperHooks, thisptr, result, RETURN_ADDRESS(), params);
    return result;
}

void FASTCALL_CONV InventoryManagerHooks::updateInventoryEquippedState(FASTCALL_THIS(csgo::InventoryManagerPOD* thisptr), std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap) noexcept
{
    globalContext->features->inventoryChanger.onItemEquip(team, slot, itemID);
    globalContext->hooks->inventoryManagerHooks.getOriginalUpdateInventoryEquippedState()(thisptr, inventory, itemID, team, slot, swap);
}

void FASTCALL_CONV CSPlayerInventoryHooks::soUpdated(FASTCALL_THIS(csgo::CSPlayerInventoryPOD* thisptr), csgo::SOID owner, csgo::SharedObjectPOD* object, int event) noexcept
{
    globalContext->features->inventoryChanger.onSoUpdated(csgo::SharedObject::from(retSpoofGadgets->client, object));
    globalContext->hooks->playerInventoryHooks.getOriginalSoUpdated()(thisptr, owner, object, event);
}

bool FASTCALL_CONV ClientHooks::dispatchUserMessage(FASTCALL_THIS(csgo::ClientPOD* thisptr), csgo::UserMessageType type, int passthroughFlags, int size, const void* data) noexcept
{
    globalContext->features->misc.dispatchUserMessageHook(type, size, data);
    if (type == csgo::UserMessageType::Text)
        globalContext->features->inventoryChanger.onUserTextMsg(*globalContext->memory, data, size);

    return globalContext->hooks->clientHooks.getOriginalDispatchUserMessage()(thisptr, type, passthroughFlags, size, data);
}

void FASTCALL_CONV ClientStateHooks::packetEnd(FASTCALL_THIS(csgo::ClientState* thisptr)) noexcept
{
    const auto soundMessages = globalContext->memory->soundMessages;
    if (soundMessages->numElements > 0) {
        for (int i = 0; i <= soundMessages->lastAlloc; ++i) {
            if (!soundMessages->isIndexUsed(i))
                continue;

            auto& soundInfo = soundMessages->memory[i].element;
            if (const char* soundName = globalContext->getOtherInterfaces().getSoundEmitter().getSoundName(soundInfo.soundIndex)) {
                globalContext->features->sound.modulateSound(soundName, soundInfo.entityIndex, soundInfo.volume);
                soundInfo.volume = std::clamp(soundInfo.volume, 0.0f, 1.0f);
            }
        }
    }

    FunctionInvoker{ retSpoofGadgets->engine, globalContext->hooks->clientStateHooks.originalPacketEnd }(thisptr);
}

#if IS_WIN32()

void* FASTCALL_CONV KeyValuesSystemHooks::allocKeyValuesMemory(FASTCALL_THIS(csgo::KeyValuesSystemPOD* thisptr), int size) noexcept
{
    if (const auto returnAddress = RETURN_ADDRESS(); returnAddress == globalContext->memory->keyValuesAllocEngine || returnAddress == globalContext->memory->keyValuesAllocClient)
        return nullptr;
    return globalContext->hooks->keyValuesSystemHooks.getOriginalAllocKeyValuesMemory()(thisptr, size);
}

extern "C" BOOL WINAPI _CRT_INIT(HMODULE moduleHandle, DWORD reason, LPVOID reserved);

DWORD WINAPI unload(LPVOID) noexcept
{
    globalContext->windowProcedureHook->uninstall();

    Sleep(100);

    globalContext->getOtherInterfaces().getInputSystem().enableInput(true);
    globalContext->gameEventListener->remove();

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    const auto moduleHandle = globalContext->moduleHandle;
    _CRT_INIT(moduleHandle, DLL_PROCESS_DETACH, nullptr);

    FreeLibraryAndExitThread(moduleHandle, 0);
}

#endif
