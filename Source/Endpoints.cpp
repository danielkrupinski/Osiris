#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32()
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#endif

#include "EventListener.h"
#include "GlobalContext.h"

#include <Platform/Macros/CallStack.h>

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

bool STDCALL_CONV createMove(LINUX_ARGS(void* thisptr, ) float inputSampleTime, csgo::UserCmd* cmd) noexcept
{
    return globalContext->createMoveHook(inputSampleTime, cmd);
}

void STDCALL_CONV doPostScreenEffects(LINUX_ARGS(void* thisptr, ) void* param) noexcept
{
    globalContext->doPostScreenEffectsHook(param);
}

float STDCALL_CONV getViewModelFov(LINUX_ARGS(void* thisptr)) noexcept
{
    return globalContext->getViewModelFovHook();
}

void STDCALL_CONV drawModelExecute(LINUX_ARGS(void* thisptr, ) void* ctx, void* state, const csgo::ModelRenderInfo& info, csgo::matrix3x4* customBoneToWorld) noexcept
{
    globalContext->drawModelExecuteHook(ctx, state, info, customBoneToWorld);
}

int FASTCALL_CONV svCheatsGetInt(void* _this) noexcept
{
    return globalContext->svCheatsGetIntHook(_this, RETURN_ADDRESS());
}

void STDCALL_CONV frameStageNotify(LINUX_ARGS(void* thisptr, ) csgo::FrameStage stage) noexcept
{
    globalContext->frameStageNotifyHook(stage);
}

int STDCALL_CONV emitSound(LINUX_ARGS(void* thisptr, ) void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const csgo::Vector& origin, const csgo::Vector& direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams) noexcept
{
    return globalContext->emitSoundHook(filter, entityIndex, channel, soundEntry, soundEntryHash, sample, volume, seed, soundLevel, flags, pitch, origin, direction, utlVecOrigins, updatePositions, soundtime, speakerentity, soundParams);
}

bool STDCALL_CONV shouldDrawFog(LINUX_ARGS(void* thisptr)) noexcept
{
    return globalContext->shouldDrawFogHook(RETURN_ADDRESS());
}

bool STDCALL_CONV shouldDrawViewModel(LINUX_ARGS(void* thisptr)) noexcept
{
    return globalContext->shouldDrawViewModelHook();
}

void STDCALL_CONV lockCursor() noexcept
{
    globalContext->lockCursorHook();
}

void STDCALL_CONV setDrawColor(LINUX_ARGS(void* thisptr, ) int r, int g, int b, int a) noexcept
{
    globalContext->setDrawColorHook(r, g, b, a, RETURN_ADDRESS());
}

void STDCALL_CONV overrideView(LINUX_ARGS(void* thisptr, ) csgo::ViewSetup* setup) noexcept
{
    globalContext->overrideViewHook(setup);
}

int STDCALL_CONV listLeavesInBox(LINUX_ARGS(void* thisptr, ) const csgo::Vector& mins, const csgo::Vector& maxs, unsigned short* list, int listMax) noexcept
{
    return globalContext->listLeavesInBoxHook(mins, maxs, list, listMax, RETURN_ADDRESS(), FRAME_ADDRESS());
}

int FASTCALL_CONV dispatchSound(csgo::SoundInfo& soundInfo) noexcept
{
    return globalContext->dispatchSoundHook(soundInfo);
}

void STDCALL_CONV render2dEffectsPreHud(LINUX_ARGS(void* thisptr, ) void* viewSetup) noexcept
{
    globalContext->render2dEffectsPreHudHook(viewSetup);
}

const csgo::DemoPlaybackParameters* STDCALL_CONV getDemoPlaybackParameters(LINUX_ARGS(void* thisptr)) noexcept
{
    return globalContext->getDemoPlaybackParametersHook(RETURN_ADDRESS());
}

bool STDCALL_CONV isPlayingDemo(LINUX_ARGS(void* thisptr)) noexcept
{
    return globalContext->isPlayingDemoHook(RETURN_ADDRESS(), FRAME_ADDRESS());
}

void STDCALL_CONV updateColorCorrectionWeights(LINUX_ARGS(void* thisptr)) noexcept
{
    globalContext->updateColorCorrectionWeightsHook();
}

float STDCALL_CONV getScreenAspectRatio(LINUX_ARGS(void* thisptr, ) int width, int height) noexcept
{
    return globalContext->getScreenAspectRatioHook(width, height);
}

void STDCALL_CONV renderSmokeOverlay(LINUX_ARGS(void* thisptr, ) bool update) noexcept
{
    globalContext->renderSmokeOverlayHook(update);
}

double STDCALL_CONV getArgAsNumber(LINUX_ARGS(void* thisptr, ) void* params, int index) noexcept
{
    return globalContext->getArgAsNumberHook(params, index, RETURN_ADDRESS());
}

const char* STDCALL_CONV getArgAsString(LINUX_ARGS(void* thisptr, ) void* params, int index) noexcept
{
    return globalContext->getArgAsStringHook(params, index, RETURN_ADDRESS());
}

void STDCALL_CONV setResultInt(LINUX_ARGS(void* thisptr, ) void* params, int result) noexcept
{
    return globalContext->setResultIntHook(params, result, RETURN_ADDRESS());
}

unsigned STDCALL_CONV getNumArgs(LINUX_ARGS(void* thisptr, ) void* params) noexcept
{
    return globalContext->getNumArgsHook(params, RETURN_ADDRESS());
}

void STDCALL_CONV updateInventoryEquippedState(LINUX_ARGS(void* thisptr, ) std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap) noexcept
{
    globalContext->updateInventoryEquippedStateHook(inventory, itemID, team, slot, swap);
}

void STDCALL_CONV soUpdated(LINUX_ARGS(void* thisptr, ) csgo::SOID owner, csgo::SharedObjectPOD* object, int event) noexcept
{
    globalContext->soUpdatedHook(owner, object, event);
}

bool STDCALL_CONV dispatchUserMessage(LINUX_ARGS(void* thisptr, ) csgo::UserMessageType type, int passthroughFlags, int size, const void* data) noexcept
{
    return globalContext->dispatchUserMessageHook(type, passthroughFlags, size, data);
}

#if IS_WIN32()

void* STDCALL_CONV allocKeyValuesMemory(LINUX_ARGS(void* thisptr, ) int size) noexcept
{
    return globalContext->allocKeyValuesMemoryHook(size, RETURN_ADDRESS());
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
