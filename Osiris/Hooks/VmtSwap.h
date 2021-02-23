#pragma once

#include <cstddef>
#include <memory>

#include "../SDK/Platform.h"

class VmtSwap {
public:
    void init(void* base) noexcept;
    void restore() noexcept
    {
        *reinterpret_cast<uintptr_t**>(base) = oldVmt;
    }

    template<typename T>
    void hookAt(std::size_t index, T fun) const noexcept
    {
        newVmt[index + dynamicCastInfoLength] = reinterpret_cast<uintptr_t>(fun);
    }

    template<typename T, typename ...Args>
    constexpr auto getOriginal(std::size_t index, Args... args) const noexcept
    {
        return reinterpret_cast<T(__thiscall*)(void*, Args...)>(oldVmt[index]);
    }

    template<typename T, std::size_t Idx, typename ...Args>
    constexpr auto callOriginal(Args... args) const noexcept
    {
        return reinterpret_cast<T(__thiscall*)(void*, Args...)>(oldVmt[Idx])(base, args...);
    }

private:
    static constexpr auto dynamicCastInfoLength = IS_WIN32() ? 1 : 2;

    void* base = nullptr;
    uintptr_t* oldVmt = nullptr;
    std::unique_ptr<uintptr_t[]> newVmt;
    std::size_t length = 0;
};
