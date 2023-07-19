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

#include "CSGO/Constants/ConVarNames.h"

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

class Hooks {
public:
#if IS_WIN32() || IS_WIN64()
    Hooks(const Memory& memory, csgo::ClientPOD* clientInterface, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, DynamicLibrary clientDll, DynamicLibrary engineDll, DynamicLibrary vstdlibDll, DynamicLibrary vguiMatSurfaceDll) noexcept
        : engineHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() }, engineInterfaces.getEngine().getPOD() }
        , clientHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() }, clientInterface }
        , clientModeHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() }, memory.clientMode }
        , clientStateHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() }, &memory.splitScreen->splitScreenPlayers[0]->client }
        , playerInventoryHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() }, memory.inventoryManager.getLocalInventory() }
        , panoramaMarshallHelperHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() }, memory.panoramaMarshallHelper }
        , viewRenderHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() }, memory.viewRender }
        , inventoryManagerHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() }, memory.inventoryManager.getPOD() }
        , bspQueryHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() }, engineInterfaces.getEngine().getBSPTreeQuery() }
        , engineSoundHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() }, std::get<csgo::EngineSoundPOD*>(engineInterfaces.getPODs()) }
        , svCheatsHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() }, interfaces.getCvar().findVar(csgo::sv_cheats) }
        , modelRenderHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() }, std::get<csgo::ModelRenderPOD*>(engineInterfaces.getPODs()) }
        , surfaceHooks{ VmtLengthCalculator{ vguiMatSurfaceDll.getCodeSection(), vguiMatSurfaceDll.getVmtSection() }, interfaces.getSurface().getPOD() }
#if IS_WIN32()
        , keyValuesSystemHooks{ VmtLengthCalculator{ vstdlibDll.getCodeSection(), vstdlibDll.getVmtSection() }, memory.keyValuesSystem }
#endif
        , memory{ memory }
    {
    }

    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;
#elif IS_LINUX()
    Hooks(std::add_pointer_t<int(SDL_Event*)> pollEvent, const Memory& memory, csgo::ClientPOD* clientInterface, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, DynamicLibrary clientDll, DynamicLibrary engineDll, DynamicLibrary vstdlibDll, DynamicLibrary vguiMatSurfaceDll) noexcept
        : sdlFunctions{ DynamicLibrary{ "libSDL2-2.0.so.0" } }
        , pollEvent{ pollEvent }
        , engineHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() }, engineInterfaces.getEngine().getPOD() }
        , clientHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() }, clientInterface }
        , clientModeHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() }, memory.clientMode }
        , clientStateHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() }, &memory.splitScreen->splitScreenPlayers[0]->client }
        , playerInventoryHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() }, memory.inventoryManager.getLocalInventory() }
        , panoramaMarshallHelperHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() }, memory.panoramaMarshallHelper }
        , viewRenderHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() }, memory.viewRender }
        , inventoryManagerHooks{ VmtLengthCalculator{ clientDll.getCodeSection(), clientDll.getVmtSection() }, memory.inventoryManager.getPOD() }
        , bspQueryHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() }, engineInterfaces.getEngine().getBSPTreeQuery() }
        , engineSoundHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() }, std::get<csgo::EngineSoundPOD*>(engineInterfaces.getPODs()) }
        , svCheatsHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() }, interfaces.getCvar().findVar(csgo::sv_cheats) }
        , modelRenderHooks{ VmtLengthCalculator{ engineDll.getCodeSection(), engineDll.getVmtSection() }, std::get<csgo::ModelRenderPOD*>(engineInterfaces.getPODs()) }
        , surfaceHooks{ VmtLengthCalculator{ vguiMatSurfaceDll.getCodeSection(), vguiMatSurfaceDll.getVmtSection() }, interfaces.getSurface().getPOD() }
        , memory{ memory }
    {
    }

    SdlFunctions sdlFunctions;
    std::add_pointer_t<int(SDL_Event*)> pollEvent;
    std::add_pointer_t<void(SDL_Window*)> swapWindow;
#endif

    void install() noexcept;
    void uninstall(Misc& misc, Glow& glow, Visuals& visuals, inventory_changer::InventoryChanger& inventoryChanger) noexcept;

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
#if IS_WIN32()
    KeyValuesSystemHooks keyValuesSystemHooks;
#endif

    const Memory& memory;
};
