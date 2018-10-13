#pragma once

#include <type_traits>

#include <d3d9.h>

#include "SDK/UserCmd.h"

class Hooks final {
public:
    Hooks();
    WNDPROC originalWndProc;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;

    std::add_pointer_t<bool __fastcall(void*, void*, float, UserCmd*)> originalCreateMove;
    std::add_pointer_t<void __stdcall()> originalLockCursor;
};

extern Hooks hooks;
