#pragma once

#include <d3d9.h>
#include <type_traits>

#include "Interfaces.h"
#include "Memory.h"
#include "SDK/Cvar.h"
#include "SDK/Engine.h"

struct SoundInfo;

class Hooks final {
public:
    Hooks() noexcept;
    void restore() noexcept;

    WNDPROC originalWndProc;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;
    std::add_pointer_t<int __fastcall(SoundInfo&)> originalDispatchSound;

    class Vmt {
    public:
        explicit Vmt(void* const) noexcept;
        void restore() noexcept;

        template<typename T>
        void hookAt(size_t index, T fun) const noexcept
        {
            if (index <= length)
                newVmt[index + 1] = reinterpret_cast<uintptr_t>(fun);
        }

        template<typename T, typename ...Args>
        constexpr auto callOriginal(size_t index, Args... args) const noexcept
        {
            return reinterpret_cast<T(__thiscall*)(void*, Args...)>(oldVmt[index])(base, args...);
        }
    private:
        static uintptr_t* findFreeDataPage(void* const, size_t) noexcept;
        static auto calculateLength(uintptr_t*) noexcept;
        void* base;
        uintptr_t* oldVmt;
        uintptr_t* newVmt;
        size_t length;
    };

    Vmt bspQuery{ interfaces.engine->getBSPTreeQuery() };
    Vmt client{ interfaces.client };
    Vmt clientMode{ memory.clientMode };
    Vmt gameEventManager{ interfaces.gameEventManager };
    Vmt modelRender{ interfaces.modelRender };
    Vmt panel{ interfaces.panel };
    Vmt sound{ interfaces.sound };
    Vmt surface{ interfaces.surface };
    Vmt svCheats{ interfaces.cvar->findVar("sv_cheats") };
    Vmt viewRender{ memory.viewRender };
};

extern Hooks hooks;
