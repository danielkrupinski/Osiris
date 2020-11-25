#pragma once

#include <memory>
#include <type_traits>

#ifdef _WIN32
#include <d3d9.h>
#include <Windows.h>
#endif

#include "Hooks/MinHook.h"
#include "Hooks/VmtHook.h"
#include "Hooks/VmtSwap.h"

#include "SDK/Platform.h"

struct SoundInfo;

// Easily switch hooking method for all hooks, choose between MinHook/VmtHook/VmtSwap
using HookType = MinHook;

class Hooks {
public:
#ifdef _WIN32
    Hooks(HMODULE moduleHandle) noexcept;

    WNDPROC originalWndProc;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;
#endif

    void install() noexcept;
    void uninstall() noexcept;

    std::add_pointer_t<int __FASTCALL(SoundInfo&)> originalDispatchSound;

    HookType bspQuery;
    HookType client;
    HookType clientMode;
    HookType engine;
    HookType modelRender;
    HookType panel;
    HookType sound;
    HookType surface;
    HookType viewRender;
    HookType svCheats;
private:
#ifdef _WIN32
    HMODULE moduleHandle;
    HWND window;
#endif
};

inline std::unique_ptr<Hooks> hooks;
