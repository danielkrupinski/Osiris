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

        template<typename T>
        void hookAt(std::size_t index, T fun)
        {
            if (index < vmtLength)
                newVmt[index] = reinterpret_cast<std::uintptr_t>(fun);
        }

        template<typename T>
        constexpr auto getOriginal(int index) noexcept
        {
            return reinterpret_cast<T>(oldVmt[index]);
        }

        void apply();
    private:
        std::size_t calculateLength(std::uintptr_t*);
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
