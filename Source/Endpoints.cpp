#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32()
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
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

#include "CSGO/Constants/UserMessages.h"
#include "CSGO/ClientClass.h"
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

namespace
{
    std::optional<GlobalContext> globalContext;
}

void initializeGlobalContext()
{
    globalContext.emplace();
}

void DefaultEventListener::fireGameEvent(csgo::GameEventPOD* eventPointer)
{
    globalContext->fireGameEventCallback(eventPointer);
}

void EventListener::fireGameEvent(csgo::GameEventPOD* event)
{
    globalContext->fireGameEventCallback(event);
}

void CDECL_CONV viewModelSequence(csgo::recvProxyData* data, void* outStruct, void* arg3) noexcept
{
    globalContext->viewModelSequenceNetvarHook(data, outStruct, arg3);
}

void CDECL_CONV spottedHook(csgo::recvProxyData* data, void* outStruct, void* arg3) noexcept
{
    globalContext->spottedHook(data, outStruct, arg3);
}

#if IS_WIN32()

LRESULT __stdcall wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    return globalContext->wndProcHook(window, msg, wParam, lParam);
}

HRESULT __stdcall reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
    return globalContext->resetHook(device, params);
}

HRESULT __stdcall present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
{
    return globalContext->presentHook(device, src, dest, windowOverride, dirtyRegion);
}

#else

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
    auto result = hooks->clientModeHooks.getOriginalCreateMove()(thisptr, inputSampleTime, cmd);

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
    hooks->clientModeHooks.getOriginalDoPostScreenEffects()(thisptr, param);
}

float FASTCALL_CONV ClientModeHooks::getViewModelFov(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept
{
    float additionalFov = globalContext->features->visuals.viewModelFov();
    if (localPlayer) {
        if (const auto activeWeapon = csgo::Entity::from(retSpoofGadgets->client, localPlayer.get().getActiveWeapon()); activeWeapon.getPOD() != nullptr && activeWeapon.getNetworkable().getClientClass()->classId == ClassId::Tablet)
            additionalFov = 0.0f;
    }

    return hooks->clientModeHooks.getOriginalGetViewModelFov()(thisptr) + additionalFov;
}

void FASTCALL_CONV ModelRenderHooks::drawModelExecute(FASTCALL_THIS(csgo::ModelRenderPOD* thisptr), void* ctx, void* state, const csgo::ModelRenderInfo& info, csgo::matrix3x4* customBoneToWorld) noexcept
{
    if (globalContext->getOtherInterfaces().getStudioRender().isForcedMaterialOverride())
        return hooks->modelRenderHooks.getOriginalDrawModelExecute()(thisptr, ctx, state, &info, customBoneToWorld);

    auto& features = globalContext->features;
    if (features->visuals.removeHands(info.model->name) || features->visuals.removeSleeves(info.model->name) || features->visuals.removeWeapons(info.model->name))
        return;

    if (!features->chams.render(features->backtrack, ctx, state, info, customBoneToWorld))
        hooks->modelRenderHooks.getOriginalDrawModelExecute()(thisptr, ctx, state, &info, customBoneToWorld);

    globalContext->getOtherInterfaces().getStudioRender().forcedMaterialOverride(nullptr);
}

int FASTCALL_CONV SvCheatsHooks::getInt(csgo::ConVarPOD* thisptr) noexcept
{
    const auto original = hooks->svCheatsHooks.getOriginalSvCheatsGetInt()(thisptr);
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

    hooks->clientHooks.getOriginalFrameStageNotify()(thisptr, stage);
}

int FASTCALL_CONV EngineSoundHooks::emitSound(FASTCALL_THIS(csgo::EngineSoundPOD* thisptr), void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const csgo::Vector& origin, const csgo::Vector& direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams) noexcept
{
    globalContext->features->sound.modulateSound(soundEntry, entityIndex, volume);
    globalContext->features->misc.autoAccept(soundEntry);

    volume = std::clamp(volume, 0.0f, 1.0f);
    return hooks->engineSoundHooks.getOriginalEmitSound()(thisptr, filter, entityIndex, channel, soundEntry, soundEntryHash, sample, volume, seed, soundLevel, flags, pitch, &origin, &direction, utlVecOrigins, updatePositions, soundtime, speakerentity, soundParams);
}

bool FASTCALL_CONV ClientModeHooks::shouldDrawFog(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept
{
#if IS_WIN32()
    if constexpr (std::is_same_v<HookType, MinHook>) {
        if (RETURN_ADDRESS() != globalContext->memory->shouldDrawFogReturnAddress)
            return hooks->clientModeHooks.getOriginalShouldDrawFog()(thisptr);
    }
#endif

    return !globalContext->features->visuals.shouldRemoveFog();
}

bool FASTCALL_CONV ClientModeHooks::shouldDrawViewModel(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept
{
    if (globalContext->features->visuals.isZoomOn() && localPlayer && localPlayer.get().fov() < 45 && localPlayer.get().fovStart() < 45)
        return false;
    return hooks->clientModeHooks.getOriginalShouldDrawViewModel()(thisptr);
}

#if IS_WIN32()
void FASTCALL_CONV SurfaceHooks::lockCursor(FASTCALL_THIS(csgo::SurfacePOD* thisptr)) noexcept
{
    if (gui->isOpen())
        return globalContext->getOtherInterfaces().getSurface().unlockCursor();
    return hooks->surfaceHooks.getOriginalLockCursor()(thisptr);
}
#endif

void FASTCALL_CONV SurfaceHooks::setDrawColor(FASTCALL_THIS(csgo::SurfacePOD* thisptr), int r, int g, int b, int a) noexcept
{
    globalContext->features->visuals.setDrawColorHook(RETURN_ADDRESS(), a);
    hooks->surfaceHooks.getOriginalSetDrawColor()(thisptr, r, g, b, a);
}

void FASTCALL_CONV ClientModeHooks::overrideView(FASTCALL_THIS(csgo::ClientMode* thisptr), csgo::ViewSetup* setup) noexcept
{
    if (localPlayer && !localPlayer.get().isScoped())
        setup->fov += globalContext->features->visuals.fov();
    setup->farZ += globalContext->features->visuals.farZ() * 10;
    hooks->clientModeHooks.getOriginalOverrideView()(thisptr, setup);
}

int FASTCALL_CONV BspQueryHooks::listLeavesInBox(FASTCALL_THIS(void* thisptr), const csgo::Vector& mins, const csgo::Vector& maxs, unsigned short* list, int listMax) noexcept
{
    if (const auto newVectors = globalContext->features->misc.listLeavesInBoxHook(RETURN_ADDRESS(), FRAME_ADDRESS()))
        return hooks->bspQueryHooks.getOriginalListLeavesInBox()(thisptr, &newVectors->first, &newVectors->second, list, listMax);
    return hooks->bspQueryHooks.getOriginalListLeavesInBox()(thisptr, &mins, &maxs, list, listMax);
}

void FASTCALL_CONV ViewRenderHooks::render2dEffectsPreHud(FASTCALL_THIS(csgo::ViewRender* thisptr), void* viewSetup) noexcept
{
    globalContext->features->visuals.applyScreenEffects();
    globalContext->features->visuals.hitEffect();
    hooks->viewRenderHooks.getOriginalRender2dEffectsPreHud()(thisptr, viewSetup);
}

const csgo::DemoPlaybackParameters* FASTCALL_CONV EngineHooks::getDemoPlaybackParameters(FASTCALL_THIS(csgo::EnginePOD* thisptr)) noexcept
{
    const auto params = hooks->engineHooks.getOriginalGetDemoPlaybackParameters()(thisptr);

    if (params)
        return globalContext->features->misc.getDemoPlaybackParametersHook(RETURN_ADDRESS(), *params);

    return params;
}

bool FASTCALL_CONV EngineHooks::isPlayingDemo(FASTCALL_THIS(csgo::EnginePOD* thisptr)) noexcept
{
    const auto result = hooks->engineHooks.getOriginalIsPlayingDemo()(thisptr);

    if (globalContext->features->misc.isPlayingDemoHook(RETURN_ADDRESS(), FRAME_ADDRESS()))
        return true;

    return result;
}

void FASTCALL_CONV ClientModeHooks::updateColorCorrectionWeights(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept
{
    hooks->clientModeHooks.getOriginalUpdateColorCorrectionWeights()(thisptr);
    globalContext->features->visuals.updateColorCorrectionWeightsHook();
}

float FASTCALL_CONV EngineHooks::getScreenAspectRatio(FASTCALL_THIS(csgo::EnginePOD* thisptr), int width, int height) noexcept
{
    if (globalContext->features->misc.aspectRatio() != 0.0f)
        return globalContext->features->misc.aspectRatio();
    return hooks->engineHooks.getOriginalGetScreenAspectRatio()(thisptr, width, height);
}

void FASTCALL_CONV ViewRenderHooks::renderSmokeOverlay(FASTCALL_THIS(csgo::ViewRender* thisptr), bool preViewModel) noexcept
{
    if (!globalContext->features->visuals.renderSmokeOverlayHook())
        hooks->viewRenderHooks.getOriginalRenderSmokeOverlay()(thisptr, preViewModel);
}

double FASTCALL_CONV PanoramaMarshallHelperHooks::getArgAsNumber(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params, int index) noexcept
{
    const auto result = hooks->panoramaMarshallHelperHooks.getOriginalGetArgAsNumber()(thisptr, params, index);
    globalContext->features->inventoryChanger.getArgAsNumberHook(static_cast<int>(result), RETURN_ADDRESS());
    return result;
}

const char* FASTCALL_CONV PanoramaMarshallHelperHooks::getArgAsString(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params, int index) noexcept
{
    const auto result = hooks->panoramaMarshallHelperHooks.getOriginalGetArgAsString()(thisptr, params, index);

    if (result)
        globalContext->features->inventoryChanger.getArgAsStringHook(*globalContext->memory, result, RETURN_ADDRESS(), params);

    return result;
}

void FASTCALL_CONV PanoramaMarshallHelperHooks::setResultInt(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params, int result) noexcept
{
    result = globalContext->features->inventoryChanger.setResultIntHook(RETURN_ADDRESS(), params, result);
    hooks->panoramaMarshallHelperHooks.getOriginalSetResultInt()(thisptr, params, result);
}

unsigned FASTCALL_CONV PanoramaMarshallHelperHooks::getNumArgs(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params) noexcept
{
    const auto result = hooks->panoramaMarshallHelperHooks.getOriginalGetNumArgs()(globalContext->memory->panoramaMarshallHelper, params);
    globalContext->features->inventoryChanger.getNumArgsHook(thisptr, result, RETURN_ADDRESS(), params);
    return result;
}

void FASTCALL_CONV InventoryManagerHooks::updateInventoryEquippedState(FASTCALL_THIS(csgo::InventoryManagerPOD* thisptr), std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap) noexcept
{
    globalContext->features->inventoryChanger.onItemEquip(team, slot, itemID);
    hooks->inventoryManagerHooks.getOriginalUpdateInventoryEquippedState()(thisptr, inventory, itemID, team, slot, swap);
}

void FASTCALL_CONV CSPlayerInventoryHooks::soUpdated(FASTCALL_THIS(csgo::CSPlayerInventoryPOD* thisptr), csgo::SOID owner, csgo::SharedObjectPOD* object, int event) noexcept
{
    globalContext->features->inventoryChanger.onSoUpdated(csgo::SharedObject::from(retSpoofGadgets->client, object));
    hooks->playerInventoryHooks.getOriginalSoUpdated()(thisptr, owner, object, event);
}

bool FASTCALL_CONV ClientHooks::dispatchUserMessage(FASTCALL_THIS(csgo::ClientPOD* thisptr), csgo::UserMessageType type, int passthroughFlags, int size, const void* data) noexcept
{
    globalContext->features->misc.dispatchUserMessageHook(type, size, data);
    if (type == csgo::UserMessageType::Text)
        globalContext->features->inventoryChanger.onUserTextMsg(*globalContext->memory, data, size);

    return hooks->clientHooks.getOriginalDispatchUserMessage()(thisptr, type, passthroughFlags, size, data);
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

    FunctionInvoker{ retSpoofGadgets->engine, hooks->clientStateHooks.originalPacketEnd }(thisptr);
}

#if IS_WIN32()

void* FASTCALL_CONV KeyValuesSystemHooks::allocKeyValuesMemory(FASTCALL_THIS(csgo::KeyValuesSystemPOD* thisptr), int size) noexcept
{
    if (const auto returnAddress = RETURN_ADDRESS(); returnAddress == globalContext->memory->keyValuesAllocEngine || returnAddress == globalContext->memory->keyValuesAllocClient)
        return nullptr;
    return hooks->keyValuesSystemHooks.getOriginalAllocKeyValuesMemory()(thisptr, size);
}

extern "C" BOOL WINAPI _CRT_INIT(HMODULE moduleHandle, DWORD reason, LPVOID reserved);

DWORD WINAPI unload(HMODULE moduleHandle) noexcept
{
    Sleep(100);

    globalContext->getOtherInterfaces().getInputSystem().enableInput(true);
    globalContext->gameEventListener->remove();

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    _CRT_INIT(moduleHandle, DLL_PROCESS_DETACH, nullptr);

    FreeLibraryAndExitThread(moduleHandle, 0);
}

#endif
