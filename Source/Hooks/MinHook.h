#pragma once

#include <cstddef>
#include <memory>

#include <Platform/CallingConventions.h>
#include <Platform/IsPlatform.h>

#if IS_WIN32()
#include <x86RetSpoof.h>
#include "../RetSpoofGadgets.h"
#endif

class MinHook {
public:
    void init(void* base) noexcept;
    void restore() noexcept {}
    void hookAt(std::size_t index, void* fun) noexcept;

    template<typename T, std::size_t Idx, typename ...Args>
    constexpr auto getOriginal(Args...) const noexcept
    {
        return reinterpret_cast<T(THISCALL_CONV*)(void*, Args...)>(originals[Idx]);
    }

    template<typename T, std::size_t Idx, typename ...Args>
    constexpr auto callOriginal(Args... args) const noexcept
    {
#if IS_WIN32()
        return retSpoofGadgets->engine.invokeThiscall<T, Args...>(std::uintptr_t(base), originals[Idx], args...);
#else
        return getOriginal<T, Idx>(args...)(base, args...);
#endif
    }

private:
    void* base;
    std::unique_ptr<uintptr_t[]> originals;
};
