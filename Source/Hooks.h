#pragma once

#include <memory>
#include <optional>
#include <type_traits>

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32() || IS_WIN64()
#include <d3d9.h>
#include <Windows.h>
#elif IS_LINUX()
#include "SdlFunctions.h"
struct SDL_Window;
union SDL_Event;
#endif

#include "Hooks/MinHook.h"
#include "Memory.h"
#include "InventoryChanger/InventoryChanger.h"
#include "Hooks/ClientHooks.h"
#include "Hooks/ClientStateHooks.h"
#include "Hooks/ClientModeHooks.h"
#include "Hooks/EngineHooks.h"
#include "Hooks/PanoramaMarshallHelperHooks.h"
#include "Hooks/ViewRenderHooks.h"
#include "Hooks/CSPlayerInventoryHooks.h"
#include "Hooks/InventoryManagerHooks.h"
#include "Hooks/BspQueryHooks.h"
#include "Hooks/EngineSoundHooks.h"
#include "Hooks/SvCheatsHooks.h"
#include "Hooks/ModelRenderHooks.h"
#include "Hooks/SurfaceHooks.h"

#if IS_WIN32() || IS_WIN64()
#include "Hooks/KeyValuesSystemHooks.h"
#include "Hooks/WindowProcedureHook.h"
#endif

#include "HookType.h"

namespace csgo
{

struct matrix3x4;
struct ModelRenderInfo;
struct SoundInfo;
struct ClientPOD;

}

class ClientInterfaces;
class OtherInterfaces;
class Glow;
class Visuals;
class Misc;

#if IS_LINUX()

int pollEvent(SDL_Event* event) noexcept;

#endif

class Hooks {
public:
#if IS_WIN32() || IS_WIN64()
    Hooks(HMODULE moduleHandle, DynamicLibrary clientDll, DynamicLibrary engineDll, DynamicLibrary vstdlibDll, DynamicLibrary vguiMatSurfaceDll) noexcept
        : windowProcedureHook{ FindWindowW(L"Valve001", nullptr) }
#if IS_WIN32()
        , keyValuesSystemHooks{ VmtLengthCalculator{ vstdlibDll.getCodeSection(), vstdlibDll.getVmtSection() } }
#endif
        , engineHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() } }
        , clientHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() } }
        , clientModeHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() } }
        , clientStateHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() } }
        , playerInventoryHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() } }
        , panoramaMarshallHelperHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() } }
        , viewRenderHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() } }
        , inventoryManagerHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() } }
        , bspQueryHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() } }
        , engineSoundHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() } }
        , svCheatsHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() } }
        , modelRenderHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() } }
        , surfaceHooks{ VmtLengthCalculator{ vguiMatSurfaceDll.getCodeSection(), vguiMatSurfaceDll.getVmtSection() } }
        , moduleHandle{ moduleHandle }
    {
    }

    WindowProcedureHook windowProcedureHook;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;
#elif IS_LINUX()
    Hooks(DynamicLibrary clientDll, DynamicLibrary engineDll, DynamicLibrary vstdlibDll, DynamicLibrary vguiMatSurfaceDll) noexcept
        : sdlFunctions{ DynamicLibrary{ "libSDL2-2.0.so.0" } }
        , engineHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() } }
        , clientHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() } }
        , clientModeHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() } }
        , clientStateHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() } }
        , playerInventoryHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() } }
        , panoramaMarshallHelperHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() } }
        , viewRenderHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() } }
        , inventoryManagerHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() } }
        , bspQueryHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() } }
        , engineSoundHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() } }
        , svCheatsHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() } }
        , modelRenderHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() } }
        , surfaceHooks{ VmtLengthCalculator{ vguiMatSurfaceDll.getCodeSection(), vguiMatSurfaceDll.getVmtSection() } }
    {
        pollEvent = *reinterpret_cast<decltype(pollEvent)*>(sdlFunctions.pollEvent);
        *reinterpret_cast<decltype(::pollEvent)**>(sdlFunctions.pollEvent) = ::pollEvent;
    }

    SdlFunctions sdlFunctions;

    std::add_pointer_t<int(SDL_Event*)> pollEvent;
    std::add_pointer_t<void(SDL_Window*)> swapWindow;
#endif

    void install(csgo::ClientPOD* clientInterface, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory) noexcept;
    void uninstall(Misc& misc, Glow& glow, const Memory& memory, Visuals& visuals, inventory_changer::InventoryChanger& inventoryChanger, csgo::ClientPOD* clientInterface, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces) noexcept;

#if IS_WIN32()
    KeyValuesSystemHooks keyValuesSystemHooks;
#endif

    EngineHooks engineHooks;
    ClientHooks clientHooks;
    ClientModeHooks clientModeHooks;
    ClientStateHooks clientStateHooks;
    CSPlayerInventoryHooks playerInventoryHooks;
    PanoramaMarshallHelperHooks panoramaMarshallHelperHooks;
    ViewRenderHooks viewRenderHooks;
    InventoryManagerHooks inventoryManagerHooks;
    BspQueryHooks bspQueryHooks;
    EngineSoundHooks engineSoundHooks;
    SvCheatsHooks svCheatsHooks;
    ModelRenderHooks modelRenderHooks;
    SurfaceHooks surfaceHooks;

private:
#if IS_WIN32() || IS_WIN64()
    HMODULE moduleHandle;
#endif
};

inline std::optional<Hooks> hooks;
