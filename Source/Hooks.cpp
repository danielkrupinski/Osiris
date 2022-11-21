#include <charconv>
#include <functional>
#include <string>

#include "imgui/imgui.h"

#include "Platform/CallStack.h"
#include "Platform/IsPlatform.h"

#if IS_WIN32()
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

#include "SDK/PODs/RenderableInfo.h"
#include "SDK/ClientClass.h"
#include "SDK/Cvar.h"
#include "SDK/Engine.h"
#include "SDK/Entity.h"
#include "SDK/EntityList.h"
#include "SDK/Constants/FrameStage.h"
#include "SDK/CSPlayerInventory.h"
#include "SDK/GameEvent.h"
#include "SDK/GlobalVars.h"
#include "SDK/InputSystem.h"
#include "SDK/ItemSchema.h"
#include "SDK/LocalPlayer.h"
#include "SDK/MaterialSystem.h"
#include "SDK/ModelRender.h"
#include "SDK/SoundInfo.h"
#include "SDK/SoundEmitter.h"
#include "SDK/StudioRender.h"
#include "SDK/Surface.h"
#include "SDK/UserCmd.h"
#include "SDK/ViewSetup.h"
#include "SDK/Constants/UserMessages.h"

#include "GlobalContext.h"
#include "Interfaces/ClientInterfaces.h"

#if IS_WIN32()

static LRESULT __stdcall wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    return globalContext->wndProcHook(window, msg, wParam, lParam);
}

static HRESULT __stdcall reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
    return globalContext->resetHook(device, params);
}

static HRESULT __stdcall present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept
{
    return globalContext->presentHook(device, src, dest, windowOverride, dirtyRegion);
}

#else

static int pollEvent(SDL_Event* event) noexcept
{
    return globalContext->pollEventHook(event);
}

static void swapWindow(SDL_Window* window) noexcept
{
    globalContext->swapWindowHook(window);
}

#endif

static bool STDCALL_CONV createMove(LINUX_ARGS(void* thisptr,) float inputSampleTime, UserCmd* cmd) noexcept
{
    return globalContext->createMoveHook(inputSampleTime, cmd);
}

static void STDCALL_CONV doPostScreenEffects(LINUX_ARGS(void* thisptr,) void* param) noexcept
{
    globalContext->doPostScreenEffectsHook(param);
}

static float STDCALL_CONV getViewModelFov(LINUX_ARGS(void* thisptr)) noexcept
{
    return globalContext->getViewModelFovHook();
}

static void STDCALL_CONV drawModelExecute(LINUX_ARGS(void* thisptr,) void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    globalContext->drawModelExecuteHook(ctx, state, info, customBoneToWorld);
}

static bool FASTCALL_CONV svCheatsGetBool(void* _this) noexcept
{
    return globalContext->svCheatsGetBoolHook(_this, RETURN_ADDRESS());
}

static void STDCALL_CONV frameStageNotify(LINUX_ARGS(void* thisptr,) csgo::FrameStage stage) noexcept
{
    globalContext->frameStageNotifyHook(stage);
}

static int STDCALL_CONV emitSound(LINUX_ARGS(void* thisptr,) void* filter, int entityIndex, int channel, const char* soundEntry, unsigned int soundEntryHash, const char* sample, float volume, int seed, int soundLevel, int flags, int pitch, const Vector& origin, const Vector& direction, void* utlVecOrigins, bool updatePositions, float soundtime, int speakerentity, void* soundParams) noexcept
{
    return globalContext->emitSoundHook(filter, entityIndex, channel, soundEntry, soundEntryHash, sample, volume, seed, soundLevel, flags, pitch, origin, direction, utlVecOrigins, updatePositions, soundtime, speakerentity, soundParams);
}

static bool STDCALL_CONV shouldDrawFog(LINUX_ARGS(void* thisptr)) noexcept
{
    return globalContext->shouldDrawFogHook(RETURN_ADDRESS());
}

static bool STDCALL_CONV shouldDrawViewModel(LINUX_ARGS(void* thisptr)) noexcept
{
    return globalContext->shouldDrawViewModelHook();
}

static void STDCALL_CONV lockCursor() noexcept
{
    globalContext->lockCursorHook();
}

static void STDCALL_CONV setDrawColor(LINUX_ARGS(void* thisptr,) int r, int g, int b, int a) noexcept
{
    globalContext->setDrawColorHook(r, g, b, a, RETURN_ADDRESS());
}

static void STDCALL_CONV overrideView(LINUX_ARGS(void* thisptr,) ViewSetup* setup) noexcept
{
    globalContext->overrideViewHook(setup);
}

static int STDCALL_CONV listLeavesInBox(LINUX_ARGS(void* thisptr, ) const Vector& mins, const Vector& maxs, unsigned short* list, int listMax) noexcept
{
    return globalContext->listLeavesInBoxHook(mins, maxs, list, listMax, RETURN_ADDRESS(), FRAME_ADDRESS());
}

static int FASTCALL_CONV dispatchSound(SoundInfo& soundInfo) noexcept
{
    return globalContext->dispatchSoundHook(soundInfo);
}

static void STDCALL_CONV render2dEffectsPreHud(LINUX_ARGS(void* thisptr,) void* viewSetup) noexcept
{
    globalContext->render2dEffectsPreHudHook(viewSetup);
}

static const DemoPlaybackParameters* STDCALL_CONV getDemoPlaybackParameters(LINUX_ARGS(void* thisptr)) noexcept
{
    return globalContext->getDemoPlaybackParametersHook(RETURN_ADDRESS());
}

static bool STDCALL_CONV isPlayingDemo(LINUX_ARGS(void* thisptr)) noexcept
{
    return globalContext->isPlayingDemoHook(RETURN_ADDRESS(), FRAME_ADDRESS());
}

static void STDCALL_CONV updateColorCorrectionWeights(LINUX_ARGS(void* thisptr)) noexcept
{
    globalContext->updateColorCorrectionWeightsHook();
}

static float STDCALL_CONV getScreenAspectRatio(LINUX_ARGS(void* thisptr,) int width, int height) noexcept
{
    return globalContext->getScreenAspectRatioHook(width, height);
}

static void STDCALL_CONV renderSmokeOverlay(LINUX_ARGS(void* thisptr,) bool update) noexcept
{
    globalContext->renderSmokeOverlayHook(update);
}

static double STDCALL_CONV getArgAsNumber(LINUX_ARGS(void* thisptr,) void* params, int index) noexcept
{
    return globalContext->getArgAsNumberHook(params, index, RETURN_ADDRESS());
}

static const char* STDCALL_CONV getArgAsString(LINUX_ARGS(void* thisptr,) void* params, int index) noexcept
{
    return globalContext->getArgAsStringHook(params, index, RETURN_ADDRESS());
}

static void STDCALL_CONV setResultInt(LINUX_ARGS(void* thisptr, ) void* params, int result) noexcept
{
    return globalContext->setResultIntHook(params, result, RETURN_ADDRESS());
}

static unsigned STDCALL_CONV getNumArgs(LINUX_ARGS(void* thisptr, ) void* params) noexcept
{
    return globalContext->getNumArgsHook(params, RETURN_ADDRESS());
}

static void STDCALL_CONV updateInventoryEquippedState(LINUX_ARGS(void* thisptr, ) std::uintptr_t inventory, csgo::ItemId itemID, csgo::Team team, int slot, bool swap) noexcept
{
    globalContext->updateInventoryEquippedStateHook(inventory, itemID, team, slot, swap);
}

static void STDCALL_CONV soUpdated(LINUX_ARGS(void* thisptr, ) SOID owner, csgo::pod::SharedObject* object, int event) noexcept
{
    globalContext->soUpdatedHook(owner, object, event);
}

static bool STDCALL_CONV dispatchUserMessage(LINUX_ARGS(void* thisptr, ) csgo::UserMessageType type, int passthroughFlags, int size, const void* data) noexcept
{
    return globalContext->dispatchUserMessageHook(type, passthroughFlags, size, data);
}

#if IS_WIN32()

static void* STDCALL_CONV allocKeyValuesMemory(LINUX_ARGS(void* thisptr, ) int size) noexcept
{
    return globalContext->allocKeyValuesMemoryHook(size, RETURN_ADDRESS());
}

Hooks::Hooks(HMODULE moduleHandle) noexcept : moduleHandle{ moduleHandle }
{
#ifndef __MINGW32__
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#endif

    window = FindWindowW(L"Valve001", nullptr);
    originalWndProc = WNDPROC(SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(&wndProc)));
}

#endif

void Hooks::install(csgo::pod::Client* clientInterface, const OtherInterfaces& interfaces, const Memory& memory) noexcept
{
#if IS_WIN32()
    originalPresent = **reinterpret_cast<decltype(originalPresent)**>(memory.present);
    **reinterpret_cast<decltype(present)***>(memory.present) = present;
    originalReset = **reinterpret_cast<decltype(originalReset)**>(memory.reset);
    **reinterpret_cast<decltype(reset)***>(memory.reset) = reset;

    if constexpr (std::is_same_v<HookType, MinHook>)
        MH_Initialize();
#else
    ImGui_ImplOpenGL3_Init();

    swapWindow = *reinterpret_cast<decltype(swapWindow)*>(sdlFunctions.swapWindow);
    *reinterpret_cast<decltype(::swapWindow)**>(sdlFunctions.swapWindow) = ::swapWindow;

#endif
    
    bspQuery.init(globalContext->getEngineInterfaces().getEngine().getBSPTreeQuery());
    bspQuery.hookAt(6, &listLeavesInBox);

    client.init(clientInterface);
    client.hookAt(37, &frameStageNotify);
    client.hookAt(38, &dispatchUserMessage);

    clientMode.init(memory.clientMode);
    clientMode.hookAt(WIN32_LINUX(17, 18), &shouldDrawFog);
    clientMode.hookAt(WIN32_LINUX(18, 19), &overrideView);
    clientMode.hookAt(WIN32_LINUX(24, 25), &createMove);
    clientMode.hookAt(WIN32_LINUX(27, 28), &shouldDrawViewModel);
    clientMode.hookAt(WIN32_LINUX(35, 36), &getViewModelFov);
    clientMode.hookAt(WIN32_LINUX(44, 45), &doPostScreenEffects);
    clientMode.hookAt(WIN32_LINUX(58, 61), &updateColorCorrectionWeights);

    engine.init(globalContext->getEngineInterfaces().getEngine().getPOD());
    engine.hookAt(82, &isPlayingDemo);
    engine.hookAt(101, &getScreenAspectRatio);
#if IS_WIN32()
    keyValuesSystem.init(memory.keyValuesSystem);
    keyValuesSystem.hookAt(2, &allocKeyValuesMemory);
#endif
    engine.hookAt(WIN32_LINUX(218, 219), &getDemoPlaybackParameters);

    inventory.init((void*)memory.inventoryManager.getLocalInventory());
    inventory.hookAt(1, &soUpdated);

    inventoryManager.init(memory.inventoryManager.getPOD());
    inventoryManager.hookAt(WIN32_LINUX(29, 30), &updateInventoryEquippedState);

    modelRender.init(globalContext->engineInterfacesPODs->modelRender);
    modelRender.hookAt(21, &drawModelExecute);

    panoramaMarshallHelper.init(memory.panoramaMarshallHelper);
    panoramaMarshallHelper.hookAt(1, &getNumArgs);
    panoramaMarshallHelper.hookAt(5, &getArgAsNumber);
    panoramaMarshallHelper.hookAt(7, &getArgAsString);
    panoramaMarshallHelper.hookAt(WIN32_LINUX(14, 11), &setResultInt);

    sound.init(globalContext->engineInterfacesPODs->sound);
    sound.hookAt(WIN32_LINUX(5, 6), &emitSound);

    surface.init(interfaces.getSurface().getPOD());
    surface.hookAt(WIN32_LINUX(15, 14), &setDrawColor);
    
    svCheats.init(interfaces.getCvar().findVar("sv_cheats"));
    svCheats.hookAt(WIN32_LINUX(13, 16), &svCheatsGetBool);

    viewRender.init(memory.viewRender);
    viewRender.hookAt(WIN32_LINUX(39, 40), &render2dEffectsPreHud);
    viewRender.hookAt(WIN32_LINUX(41, 42), &renderSmokeOverlay);

#if IS_WIN32()
    if (DWORD oldProtection; VirtualProtect(memory.dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection)) {
#else
    if (const auto addressPageAligned = std::uintptr_t(memory.dispatchSound) - std::uintptr_t(memory.dispatchSound) % sysconf(_SC_PAGESIZE);
        mprotect((void*)addressPageAligned, 4, PROT_READ | PROT_WRITE | PROT_EXEC) == 0) {
#endif
        originalDispatchSound = decltype(originalDispatchSound)(uintptr_t(memory.dispatchSound + 1) + *memory.dispatchSound);
        *memory.dispatchSound = uintptr_t(&dispatchSound) - uintptr_t(memory.dispatchSound + 1);
#if IS_WIN32()
        VirtualProtect(memory.dispatchSound, 4, oldProtection, nullptr);
#endif
    }

#if IS_WIN32()
    surface.hookAt(67, &lockCursor);

    if constexpr (std::is_same_v<HookType, MinHook>)
        MH_EnableHook(MH_ALL_HOOKS);
#endif
}

#if IS_WIN32()

extern "C" BOOL WINAPI _CRT_INIT(HMODULE moduleHandle, DWORD reason, LPVOID reserved);

static DWORD WINAPI unload(HMODULE moduleHandle) noexcept
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

void Hooks::uninstall(const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory) noexcept
{
    Misc::updateEventListeners(globalContext->getEngineInterfaces(), true);
    globalContext->visuals->updateEventListeners(true);

#if IS_WIN32()
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

    Glow::clearCustomObjects(memory);
    inventory_changer::InventoryChanger::instance(interfaces, memory).reset(interfaces, memory);

#if IS_WIN32()
    keyValuesSystem.restore();

    SetWindowLongPtrW(window, GWLP_WNDPROC, LONG_PTR(originalWndProc));
    **reinterpret_cast<void***>(memory.present) = originalPresent;
    **reinterpret_cast<void***>(memory.reset) = originalReset;

    if (DWORD oldProtection; VirtualProtect(memory.dispatchSound, 4, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        *memory.dispatchSound = uintptr_t(originalDispatchSound) - uintptr_t(memory.dispatchSound + 1);
        VirtualProtect(memory.dispatchSound, 4, oldProtection, nullptr);
    }

    if (HANDLE thread = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(unload), moduleHandle, 0, nullptr))
        CloseHandle(thread);
#else
    *reinterpret_cast<decltype(pollEvent)*>(sdlFunctions.pollEvent) = pollEvent;
    *reinterpret_cast<decltype(swapWindow)*>(sdlFunctions.swapWindow) = swapWindow;
#endif
}

void Hooks::callOriginalDrawModelExecute(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept
{
    modelRender.callOriginal<void, 21>(ctx, state, std::cref(info), customBoneToWorld);
}

#if !IS_WIN32()

Hooks::Hooks() noexcept
    : sdlFunctions{ linux_platform::SharedObject{ linux_platform::DynamicLibraryWrapper{}, "libSDL2-2.0.so.0" }.getView() }
{
    pollEvent = *reinterpret_cast<decltype(pollEvent)*>(sdlFunctions.pollEvent);
    *reinterpret_cast<decltype(::pollEvent)**>(sdlFunctions.pollEvent) = ::pollEvent;
}

#endif
