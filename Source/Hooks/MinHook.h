#pragma once

#include <cstddef>
#include <memory>

#include "../SDK/Platform.h"

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
        return getOriginal<T, Idx>(args...)(base, args...);
    }

private:
    void* base;
    std::unique_ptr<uintptr_t[]> originals;
};
