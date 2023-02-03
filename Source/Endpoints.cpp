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

static std::optional<GlobalContext> globalContext;

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

bool FASTCALL_CONV createMove(FASTCALL_THIS(csgo::ClientMode* thisptr), float inputSampleTime, csgo::UserCmd* cmd) noexcept
{
    return globalContext->createMoveHook(thisptr, inputSampleTime, cmd);
}

void FASTCALL_CONV doPostScreenEffects(FASTCALL_THIS(csgo::ClientMode* thisptr), void* param) noexcept
{
    globalContext->doPostScreenEffectsHook(thisptr, param);
}

float FASTCALL_CONV getViewModelFov(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept
{
    return globalContext->getViewModelFovHook(thisptr);
}

void FASTCALL_CONV drawModelExecute(FASTCALL_THIS(csgo::ModelRenderPOD* thisptr), void* ctx, void* state, const csgo::ModelRenderInfo& info, csgo::matrix3x4* customBoneToWorld) noexcept
{
    globalContext->drawModelExecuteHook(thisptr, ctx, state, info, customBoneToWorld);
}

int FASTCALL_CONV svCheatsGetInt(csgo::ConVarPOD* thisptr) noexcept
{
    return globalContext->svCheatsGetIntHook(thisptr, RETURN_ADDRESS());
}

void FASTCALL_CONV frameStageNotify(FASTCALL_THIS(csgo::ClientPOD* thisptr), csgo::FrameStage stage) noexcept
{
    globalContext->frameStageNotifyHook(thisptr, stage);
}

int FASTCALL_CONV emitSound(FASTCALL_THIS(csgo::EngineSoundPOD* thisptr), void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const csgo::Vector& origin, const csgo::Vector& direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams) noexcept
{
    return globalContext->emitSoundHook(thisptr, filter, entityIndex, channel, soundEntry, soundEntryHash, sample, volume, seed, soundLevel, flags, pitch, origin, direction, utlVecOrigins, updatePositions, soundtime, speakerentity, soundParams);
}

bool FASTCALL_CONV shouldDrawFog(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept
{
    return globalContext->shouldDrawFogHook(thisptr, RETURN_ADDRESS());
}

bool FASTCALL_CONV shouldDrawViewModel(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept
{
    return globalContext->shouldDrawViewModelHook(thisptr);
}

#if IS_WIN32()
void FASTCALL_CONV lockCursor(FASTCALL_THIS(csgo::SurfacePOD* thisptr)) noexcept
{
    globalContext->lockCursorHook(thisptr);
}
#endif

void FASTCALL_CONV setDrawColor(FASTCALL_THIS(csgo::SurfacePOD* thisptr), int r, int g, int b, int a) noexcept
{
    globalContext->setDrawColorHook(thisptr, r, g, b, a, RETURN_ADDRESS());
}

void FASTCALL_CONV overrideView(FASTCALL_THIS(csgo::ClientMode* thisptr), csgo::ViewSetup* setup) noexcept
{
    globalContext->overrideViewHook(thisptr, setup);
}

int FASTCALL_CONV listLeavesInBox(FASTCALL_THIS(void* thisptr), const csgo::Vector& mins, const csgo::Vector& maxs, unsigned short* list, int listMax) noexcept
{
    return globalContext->listLeavesInBoxHook(thisptr, mins, maxs, list, listMax, RETURN_ADDRESS(), FRAME_ADDRESS());
}

int FASTCALL_CONV dispatchSound(csgo::SoundInfo& soundInfo) noexcept
{
    return globalContext->dispatchSoundHook(soundInfo);
}

void FASTCALL_CONV render2dEffectsPreHud(FASTCALL_THIS(csgo::ViewRender* thisptr), void* viewSetup) noexcept
{
    globalContext->render2dEffectsPreHudHook(thisptr, viewSetup);
}

const csgo::DemoPlaybackParameters* FASTCALL_CONV getDemoPlaybackParameters(FASTCALL_THIS(csgo::EnginePOD* thisptr)) noexcept
{
    return globalContext->getDemoPlaybackParametersHook(thisptr, RETURN_ADDRESS());
}

bool FASTCALL_CONV isPlayingDemo(FASTCALL_THIS(csgo::EnginePOD* thisptr)) noexcept
{
    return globalContext->isPlayingDemoHook(thisptr, RETURN_ADDRESS(), FRAME_ADDRESS());
}

void FASTCALL_CONV updateColorCorrectionWeights(FASTCALL_THIS(csgo::ClientMode* thisptr)) noexcept
{
    globalContext->updateColorCorrectionWeightsHook(thisptr);
}

float FASTCALL_CONV getScreenAspectRatio(FASTCALL_THIS(csgo::EnginePOD* thisptr), int width, int height) noexcept
{
    return globalContext->getScreenAspectRatioHook(thisptr, width, height);
}

void FASTCALL_CONV renderSmokeOverlay(FASTCALL_THIS(csgo::ViewRender* thisptr), bool update) noexcept
{
    globalContext->renderSmokeOverlayHook(thisptr, update);
}

double FASTCALL_CONV getArgAsNumber(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params, int index) noexcept
{
    return globalContext->getArgAsNumberHook(thisptr, params, index, RETURN_ADDRESS());
}

const char* FASTCALL_CONV getArgAsString(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params, int index) noexcept
{
    return globalContext->getArgAsStringHook(thisptr, params, index, RETURN_ADDRESS());
}

void FASTCALL_CONV setResultInt(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params, int result) noexcept
{
    return globalContext->setResultIntHook(thisptr, params, result, RETURN_ADDRESS());
}

unsigned FASTCALL_CONV getNumArgs(FASTCALL_THIS(csgo::PanoramaMarshallHelperPOD* thisptr), void* params) noexcept
{
    return globalContext->getNumArgsHook(thisptr, params, RETURN_ADDRESS());
}

void FASTCALL_CONV updateInventoryEquippedState(FASTCALL_THIS(csgo::InventoryManagerPOD* thisptr), std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap) noexcept
{
    globalContext->updateInventoryEquippedStateHook(thisptr, inventory, itemID, team, slot, swap);
}

void FASTCALL_CONV soUpdated(FASTCALL_THIS(csgo::CSPlayerInventoryPOD* thisptr), csgo::SOID owner, csgo::SharedObjectPOD* object, int event) noexcept
{
    globalContext->soUpdatedHook(thisptr, owner, object, event);
}

bool FASTCALL_CONV dispatchUserMessage(FASTCALL_THIS(csgo::ClientPOD* thisptr), csgo::UserMessageType type, int passthroughFlags, int size, const void* data) noexcept
{
    return globalContext->dispatchUserMessageHook(thisptr, type, passthroughFlags, size, data);
}

#if IS_WIN32()

void* FASTCALL_CONV allocKeyValuesMemory(FASTCALL_THIS(csgo::KeyValuesSystemPOD* thisptr), int size) noexcept
{
    return globalContext->allocKeyValuesMemoryHook(thisptr, size, RETURN_ADDRESS());
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
