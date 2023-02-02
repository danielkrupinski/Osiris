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
#include "Endpoints.h"

#if IS_WIN32()

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

void Hooks::install(csgo::ClientPOD* clientInterface, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory) noexcept
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
    
    bspQueryHooks.install(engineInterfaces.getEngine().getBSPTreeQuery());

#if IS_WIN32()
    keyValuesSystemHooks.install(memory.keyValuesSystem);
#endif

    engineHooks.install(engineInterfaces.getEngine().getPOD());
    clientHooks.install(clientInterface);
    clientModeHooks.install(memory.clientMode);
    playerInventoryHooks.install(memory.inventoryManager.getLocalInventory());
    inventoryManagerHooks.install(memory.inventoryManager.getPOD());
    engineSoundHooks.install(engineInterfaces.getPODs().sound);
    modelRenderHooks.install(engineInterfaces.getPODs().modelRender);
    panoramaMarshallHelperHooks.install(memory.panoramaMarshallHelper);
    surfaceHooks.install(interfaces.getSurface().getPOD());
    svCheatsHooks.install(interfaces.getCvar().findVar(csgo::sv_cheats));
    viewRenderHooks.install(memory.viewRender);

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
    if constexpr (std::is_same_v<HookType, MinHook>)
        MH_EnableHook(MH_ALL_HOOKS);
#endif
}

void Hooks::uninstall(Misc& misc, Glow& glow, const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, Visuals& visuals, inventory_changer::InventoryChanger& inventoryChanger) noexcept
{
    misc.updateEventListeners(engineInterfaces, true);
    visuals.updateEventListeners(true);

#if IS_WIN32()
    if constexpr (std::is_same_v<HookType, MinHook>) {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }
#endif

    engineHooks.uninstall();
    clientHooks.uninstall();
    clientModeHooks.uninstall();
    panoramaMarshallHelperHooks.uninstall();
    viewRenderHooks.uninstall();
    playerInventoryHooks.uninstall();
    inventoryManagerHooks.uninstall();
    bspQueryHooks.uninstall();
    engineSoundHooks.uninstall();
    svCheatsHooks.uninstall();
    modelRenderHooks.uninstall();
    surfaceHooks.uninstall();

    Netvars::restore();

    glow.clearCustomObjects();
    inventoryChanger.reset(memory);

#if IS_WIN32()
    keyValuesSystemHooks.uninstall();

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

#if !IS_WIN32()

Hooks::Hooks() noexcept
    : sdlFunctions{ linux_platform::SharedObject{ linux_platform::DynamicLibraryWrapper{}, "libSDL2-2.0.so.0" }.getView() }
{
    pollEvent = *reinterpret_cast<decltype(pollEvent)*>(sdlFunctions.pollEvent);
    *reinterpret_cast<decltype(::pollEvent)**>(sdlFunctions.pollEvent) = ::pollEvent;
}

#endif
