#pragma once

#include <type_traits>

#include <d3d9.h>

#include "SDK/ClientMode.h"
#include "SDK/UserCmd.h"
#include "VmtHook.h"

class Hooks final {
public:
    Hooks();
    WNDPROC originalWndProc;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;

    class Vmt {
    public:
        Vmt(void*);
    private:
        void apply();
        std::size_t calculateLength(std::uintptr_t*);
        std::uintptr_t* findFreeDataPage(const std::string&, std::size_t);
        void* classBase;
        std::uintptr_t* oldVmt;
        std::uintptr_t* newVmt;
        std::size_t vmtLength;
    };

    VmtHook surface;
    VmtHook clientMode;
    VmtHook modelRender;
    VmtHook client;
};

extern Hooks hooks;
