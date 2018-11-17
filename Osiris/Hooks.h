#pragma once

#include <type_traits>

#include <d3d9.h>

#include "Interfaces.h"
#include "Memory.h"
#include "SDK/ClientMode.h"
#include "SDK/UserCmd.h"

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
        void hookAt(std::size_t index, T fun) const noexcept
        {
            if (index < vmtLength)
                newVmt[index] = reinterpret_cast<std::uintptr_t>(fun);
        }

        template<typename T>
        constexpr auto getOriginal(std::size_t index) const noexcept
        {
            return reinterpret_cast<T>(oldVmt[index]);
        }

        void apply();
        std::uintptr_t* findFreeDataPage(const std::string&, std::size_t);
    private:
        std::size_t calculateLength(std::uintptr_t*) const;
        void* classBase;
        std::uintptr_t* oldVmt;
        std::uintptr_t* newVmt;
        std::size_t vmtLength;
    };

    Vmt surface{ interfaces.surface };
    Vmt clientMode{ memory.clientMode };
};

extern Hooks hooks;
