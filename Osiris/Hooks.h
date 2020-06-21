#pragma once

#include <d3d9.h>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <Windows.h>

#include "Hooks/MinHook.h"
#include "Hooks/VmtSwap.h"

struct SoundInfo;

class Hooks {
public:
    Hooks(HMODULE module) noexcept;

    void install() noexcept;
    void uninstall() noexcept;

    WNDPROC originalWndProc;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;
    std::add_pointer_t<int __fastcall(SoundInfo&)> originalDispatchSound;

    VmtSwap svCheats;

    MinHook bspQuery;
    MinHook client;
    MinHook clientMode;
    MinHook engine;
    MinHook gameEventManager;
    MinHook modelRender;
    MinHook panel;
    MinHook sound;
    MinHook surface;
    MinHook viewRender;
private:
    HMODULE module;
    HWND window;
};

inline std::unique_ptr<Hooks> hooks;
