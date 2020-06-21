#pragma once

#include <d3d9.h>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <Windows.h>

#include "MinHook/MinHook.h"

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

    class Vmt {
    public:
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

    // TODO: Make an interface class to easily switch between hooking methods
    class MinHook {
    public:
        bool init(void* const) noexcept;

        void hookAt(std::size_t index, void* fun) noexcept
        {
            void* orig;
            MH_CreateHook((*reinterpret_cast<void***>(base))[index], fun, &orig);
            originals[index] = std::uintptr_t(orig);
        }

        template <typename T, std::size_t Idx, typename ...Args>
        auto callOriginal(Args... args) noexcept
        {
            return reinterpret_cast<T(__thiscall*)(void*, Args...)>(originals[Idx])(base, args...);
        }

        template <typename T, typename ...Args>
        auto getOriginal(std::size_t index, Args... args) noexcept
        {
            return reinterpret_cast<T(__thiscall*)(void*, Args...)>(originals[index]);
        }

        void restore() noexcept {}
    private:
        void* base = nullptr;
        std::unordered_map<std::size_t, std::uintptr_t> originals;
    };

    Vmt svCheats;

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
