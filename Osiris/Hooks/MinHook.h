#pragma once

#include <cassert>
#include <cstddef>
#include <memory>

class MinHook {
public:
    void init(void* base) noexcept;
    void restore() noexcept {}
    void hookAt(std::size_t index, void* fun) noexcept;

    template <typename T, std::size_t Idx, typename ...Args>
    auto callOriginal(Args... args) const noexcept
    {
        return reinterpret_cast<T(__thiscall*)(void*, Args...)>(originals[Idx])(base, args...);
    }

    template <typename T, typename ...Args>
    auto getOriginal(std::size_t index, Args... args) const noexcept
    {
        return reinterpret_cast<T(__thiscall*)(void*, Args...)>(originals[index]);
    }
private:
    void* base;
    std::unique_ptr<uintptr_t[]> originals;
};
