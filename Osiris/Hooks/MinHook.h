#pragma once

#include <cassert>
#include <cstddef>
#include <unordered_map>

class MinHook {
public:
    void init(void* base) noexcept;
    void restore() noexcept {}
    void hookAt(std::size_t index, void* fun) noexcept;

    template <typename T, std::size_t Idx, typename ...Args>
    auto callOriginal(Args... args) const noexcept
    {
        const auto it = originals.find(Idx);
        if (it != originals.cend())
            return reinterpret_cast<T(__thiscall*)(void*, Args...)>(it->second)(base, args...);
        assert(false);
        std::exit(1);
    }

    template <typename T, typename ...Args>
    auto getOriginal(std::size_t index, Args... args) const noexcept
    {
        const auto it = originals.find(index);
        if (it != originals.cend())
            return reinterpret_cast<T(__thiscall*)(void*, Args...)>(it->second);
        assert(false);
        std::exit(1);
    }
private:
    void* base;
    std::unordered_map<std::size_t, uintptr_t> originals;
};
