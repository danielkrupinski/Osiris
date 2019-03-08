#pragma once

#include <type_traits>

#include <d3d9.h>

#include "Interfaces.h"
#include "Memory.h"
#include "SDK/UserCmd.h"

class Hooks final {
public:
    Hooks();
    WNDPROC originalWndProc;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;

    class Vmt {
    public:
        explicit Vmt(void* const);

        template<typename T>
        void hookAt(size_t index, T fun) const noexcept
        {
            if (index <= length)
                newVmt[index + 1] = reinterpret_cast<uintptr_t>(fun);
        }

        template<typename T>
        constexpr auto getOriginal(std::size_t index) const noexcept
        {
            return reinterpret_cast<T>(oldVmt[index]);
        }
    private:
        static auto findFreeDataPage(void* const, size_t);
        static auto calculateLength(uintptr_t*) noexcept;
        uintptr_t* oldVmt;
        uintptr_t* newVmt;
        size_t length;
    };

    Vmt client{ interfaces.client };
    Vmt clientMode{ memory.clientMode };
    Vmt modelRender{ interfaces.modelRender };
    Vmt panel{ interfaces.panel };
    Vmt sound{ interfaces.sound };
    Vmt svCheats{ interfaces.cvar->findVar("sv_cheats") };
};

extern Hooks hooks;
