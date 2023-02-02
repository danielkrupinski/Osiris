#pragma once

#include <memory>
#include <optional>
#include <type_traits>

#include <Platform/Macros/IsPlatform.h>

#if IS_WIN32()
#include <d3d9.h>
#include <Windows.h>
#elif IS_LINUX()
#include "SdlFunctions.h"
struct SDL_Window;
union SDL_Event;
#endif

#include "Hooks/MinHook.h"
#include "Hooks/VmtHook.h"
#include "Hooks/VmtSwap.h"
#include "Memory.h"
#include "InventoryChanger/InventoryChanger.h"
#include "Hooks/ClientHooks.h"
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

#if IS_WIN32()
#include "Hooks/KeyValuesSystemHooks.h"
#endif

namespace csgo
{

class matrix3x4;
struct ModelRenderInfo;
struct SoundInfo;
struct ClientPOD;

}

class ClientInterfaces;
class OtherInterfaces;
class Glow;
class Visuals;
class Misc;

#if IS_WIN32()
// Easily switch hooking method for all hooks, choose between MinHook/VmtHook/VmtSwap
using HookType = MinHook;
#else
using HookType = VmtSwap;
#endif

class Hooks {
public:
#if IS_WIN32()
    Hooks(HMODULE moduleHandle) noexcept;

    WNDPROC originalWndProc;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;
#else
    Hooks() noexcept;

    SdlFunctions sdlFunctions;

    std::add_pointer_t<int(SDL_Event*)> pollEvent;
    std::add_pointer_t<void(SDL_Window*)> swapWindow;
#endif

    void install(csgo::ClientPOD* clientInterface, const EngineInterfaces& engineInterfaces, const OtherInterfaces& interfaces, const Memory& memory) noexcept;
    void uninstall(Misc& misc, Glow& glow, const EngineInterfaces& engineInterfaces, const ClientInterfaces& clientInterfaces, const OtherInterfaces& interfaces, const Memory& memory, Visuals& visuals, inventory_changer::InventoryChanger& inventoryChanger) noexcept;

    std::add_pointer_t<int FASTCALL_CONV(csgo::SoundInfo&)> originalDispatchSound;

#if IS_WIN32()
    KeyValuesSystemHooks<HookType> keyValuesSystemHooks;
#endif

    EngineHooks<HookType> engineHooks;
    ClientHooks<HookType> clientHooks;
    ClientModeHooks<HookType> clientModeHooks;
    CSPlayerInventoryHooks<HookType> playerInventoryHooks;
    PanoramaMarshallHelperHooks<HookType> panoramaMarshallHelperHooks;
    ViewRenderHooks<HookType> viewRenderHooks;
    InventoryManagerHooks<HookType> inventoryManagerHooks;
    BspQueryHooks<HookType> bspQueryHooks;
    EngineSoundHooks<HookType> engineSoundHooks;
    SvCheatsHooks<HookType> svCheatsHooks;
    ModelRenderHooks<HookType> modelRenderHooks;
    SurfaceHooks<HookType> surfaceHooks;

private:
#if IS_WIN32()
    HMODULE moduleHandle;
    HWND window;
#endif
};

inline std::optional<Hooks> hooks;
