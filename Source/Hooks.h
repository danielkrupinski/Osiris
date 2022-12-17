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
    void callOriginalDrawModelExecute(void* ctx, void* state, const csgo::ModelRenderInfo& info, csgo::matrix3x4* customBoneToWorld) noexcept;

    std::add_pointer_t<int FASTCALL_CONV(csgo::SoundInfo&)> originalDispatchSound;

    HookType bspQuery;
    HookType client;
    HookType clientMode;
    HookType engine;
    HookType inventory;
    HookType inventoryManager;
    HookType modelRender;
    HookType panoramaMarshallHelper;
    HookType sound;
    HookType surface;
    HookType viewRender;
    HookType svCheats;

#if IS_WIN32()
    HookType keyValuesSystem;
#endif
private:
#if IS_WIN32()
    HMODULE moduleHandle;
    HWND window;
#endif
};

inline std::optional<Hooks> hooks;
