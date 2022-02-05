#pragma once

#include <memory>
#include <type_traits>

#ifdef _WIN32
#include <d3d9.h>
#include <Windows.h>
#elif __linux__
struct SDL_Window;
union SDL_Event;
#endif

#include "Hooks/MinHook.h"
#include "Hooks/VmtHook.h"
#include "Hooks/VmtSwap.h"

#include "SDK/Platform.h"

class matrix3x4;
struct ModelRenderInfo;
struct SoundInfo;

#ifdef _WIN32
// Easily switch hooking method for all hooks, choose between MinHook/VmtHook/VmtSwap
using HookType = MinHook;
#else
using HookType = VmtSwap;
#endif

class Hooks {
public:
#ifdef _WIN32
    Hooks(HMODULE moduleHandle) noexcept;

    WNDPROC originalWndProc;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;
#else
    Hooks() noexcept;

    std::add_pointer_t<int(SDL_Event*)> pollEvent;
    std::add_pointer_t<void(SDL_Window*)> swapWindow;
#endif

    void install() noexcept;
    void uninstall() noexcept;
    void callOriginalDrawModelExecute(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) noexcept;

    std::add_pointer_t<int FASTCALL_CONV(SoundInfo&)> originalDispatchSound;

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

#ifdef _WIN32
    HookType keyValuesSystem;
#endif
private:
#ifdef _WIN32
    HMODULE moduleHandle;
    HWND window;
#endif
};

inline std::unique_ptr<Hooks> hooks;
