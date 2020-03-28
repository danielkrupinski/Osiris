#pragma once

#include <d3d9.h>
#include <memory>
#include <type_traits>
#include <Windows.h>

#include "Interfaces.h"
#include "Memory.h"
#include "SDK/Cvar.h"
#include "SDK/Engine.h"

struct SoundInfo;

class Hooks {
public:
    Hooks(HMODULE cheatModule);

    void restore() noexcept;

    WNDPROC originalWndProc;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;
    std::add_pointer_t<int __fastcall(SoundInfo&)> originalDispatchSound;

    class Vmt {
    public:
        Vmt(void* const base) noexcept
        {
            init(base);
        }

        Vmt() = default;
        bool init(void* const) noexcept;
        void restore() noexcept;

        template<typename T>
        void hookAt(size_t index, T fun) const noexcept
        {
            if (index <= length)
                newVmt[index + 1] = reinterpret_cast<uintptr_t>(fun);
        }

        template<typename T, std::size_t Idx, typename ...Args>
        constexpr auto callOriginal(Args... args) const noexcept
        {
            return reinterpret_cast<T(__thiscall*)(void*, Args...)>(oldVmt[Idx])(base, args...);
        }

        template<typename T, typename ...Args>
        constexpr auto getOriginal(size_t index, Args... args) const noexcept
        {
            return reinterpret_cast<T(__thiscall*)(void*, Args...)>(oldVmt[index]);
        }
    private:
        static uintptr_t* findFreeDataPage(void* const, size_t) noexcept;
        static auto calculateLength(uintptr_t*) noexcept;
        void* base = nullptr;
        uintptr_t* oldVmt = nullptr;
        uintptr_t* newVmt = nullptr;
        size_t length = 0;
    };

    Vmt bspQuery{ interfaces->engine->getBSPTreeQuery() };
    Vmt client{ interfaces->client };
    Vmt clientMode{ memory->clientMode };
    Vmt engine{ interfaces->engine };
    Vmt gameEventManager{ interfaces->gameEventManager };
    Vmt modelRender{ interfaces->modelRender };
    Vmt panel{ interfaces->panel };
    Vmt sound{ interfaces->sound };
    Vmt surface{ interfaces->surface };
    Vmt svCheats{ interfaces->cvar->findVar("sv_cheats") };
    Vmt viewRender{ memory->viewRender };
private:
    HMODULE module;
};

inline std::unique_ptr<Hooks> hooks;
