#include <charconv>
#include <functional>
#include <string>

#include "imgui/imgui.h"

#include "Platform/Macros/CallStack.h"
#include "Platform/Macros/IsPlatform.h"

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

#include "CSGO/PODs/RenderableInfo.h"
#include "CSGO/ClientClass.h"
#include "CSGO/Cvar.h"
#include "CSGO/Engine.h"
#include "CSGO/Entity.h"
#include "CSGO/EntityList.h"
#include "CSGO/Constants/ConVarNames.h"
#include "CSGO/Constants/FrameStage.h"
#include "CSGO/CSPlayerInventory.h"
#include "CSGO/GameEvent.h"
#include "CSGO/GlobalVars.h"
#include "CSGO/InputSystem.h"
#include "CSGO/ItemSchema.h"
#include "CSGO/LocalPlayer.h"
#include "CSGO/MaterialSystem.h"
#include "CSGO/ModelRender.h"
#include "CSGO/SoundInfo.h"
#include "CSGO/SoundEmitter.h"
#include "CSGO/StudioRender.h"
#include "CSGO/Surface.h"
#include "CSGO/UserCmd.h"
#include "CSGO/ViewSetup.h"
#include "CSGO/Constants/UserMessages.h"

#include "GlobalContext.h"
#include "Interfaces/ClientInterfaces.h"

#if IS_WIN32()

HRESULT __stdcall reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept;
HRESULT __stdcall present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion) noexcept;

DWORD WINAPI unload(LPVOID) noexcept;
#elif IS_LINUX()

int pollEvent(SDL_Event* event) noexcept;
void swapWindow(SDL_Window* window) noexcept;

#endif

void Hooks::install() noexcept
{
#if IS_WIN32()
    originalPresent = **reinterpret_cast<decltype(originalPresent)**>(memory.present);
    **reinterpret_cast<decltype(present)***>(memory.present) = present;
    originalReset = **reinterpret_cast<decltype(originalReset)**>(memory.reset);
    **reinterpret_cast<decltype(reset)***>(memory.reset) = reset;

    if constexpr (std::is_same_v<HookType, MinHook>)
        MH_Initialize();
#elif IS_LINUX()
    ImGui_ImplOpenGL3_Init();

    swapWindow = *reinterpret_cast<decltype(swapWindow)*>(sdlFunctions.swapWindow);
    *reinterpret_cast<decltype(::swapWindow)**>(sdlFunctions.swapWindow) = ::swapWindow;

#endif
    
    bspQueryHooks.incrementReferenceCount();
    bspQueryHooks.update();
    clientHooks.incrementReferenceCount();
    clientHooks.update();
    clientModeHooks.incrementReferenceCount();
    clientModeHooks.update();
    clientStateHooks.incrementReferenceCount();
    clientStateHooks.update();
    playerInventoryHooks.incrementReferenceCount();
    playerInventoryHooks.update();
    engineHooks.incrementReferenceCount();
    engineHooks.update();
    engineSoundHooks.incrementReferenceCount();
    engineSoundHooks.update();
    inventoryManagerHooks.incrementReferenceCount();
    inventoryManagerHooks.update();
    modelRenderHooks.incrementReferenceCount();
    modelRenderHooks.update();
    panoramaMarshallHelperHooks.incrementReferenceCount();
    panoramaMarshallHelperHooks.update();
    surfaceHooks.incrementReferenceCount();
    surfaceHooks.update();
    svCheatsHooks.incrementReferenceCount();
    svCheatsHooks.update();
    viewRenderHooks.incrementReferenceCount();
    viewRenderHooks.update();

#if IS_WIN32()
    keyValuesSystemHooks.incrementReferenceCount();
    keyValuesSystemHooks.update();
#endif

#if IS_WIN32()
    if constexpr (std::is_same_v<HookType, MinHook>)
        MH_EnableHook(MH_ALL_HOOKS);
#endif
}

void Hooks::uninstall(Misc& misc, Glow& glow, Visuals& visuals, inventory_changer::InventoryChanger& inventoryChanger) noexcept
{
    misc.updateEventListeners(true);
    visuals.updateEventListeners(true);

#if IS_WIN32()
    if constexpr (std::is_same_v<HookType, MinHook>) {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }
#endif

    engineHooks.forceUninstall();
    clientHooks.forceUninstall();
    clientModeHooks.forceUninstall();
    clientStateHooks.forceUninstall();
    panoramaMarshallHelperHooks.forceUninstall();
    viewRenderHooks.forceUninstall();
    playerInventoryHooks.forceUninstall();
    inventoryManagerHooks.forceUninstall();
    bspQueryHooks.forceUninstall();
    engineSoundHooks.forceUninstall();
    svCheatsHooks.forceUninstall();
    modelRenderHooks.forceUninstall();
    surfaceHooks.forceUninstall();

    Netvars::restore();

    glow.clearCustomObjects();
    inventoryChanger.reset(memory);

#if IS_WIN32()
    keyValuesSystemHooks.forceUninstall();

    **reinterpret_cast<void***>(memory.present) = originalPresent;
    **reinterpret_cast<void***>(memory.reset) = originalReset;

    if (HANDLE thread = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(unload), 0, 0, nullptr))
        CloseHandle(thread);
#elif IS_LINUX()
    *reinterpret_cast<decltype(pollEvent)*>(sdlFunctions.pollEvent) = pollEvent;
    *reinterpret_cast<decltype(swapWindow)*>(sdlFunctions.swapWindow) = swapWindow;
#endif
}
