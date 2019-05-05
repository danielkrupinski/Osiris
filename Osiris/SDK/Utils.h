#pragma once

#include <cstdint>

template<typename T, typename ...Args>
constexpr auto callVirtualMethod(void* classBase, int index, Args... args) noexcept
{
    return reinterpret_cast<T(__thiscall*)(void*, Args...)>((*reinterpret_cast<uintptr_t**>(classBase))[index])(classBase, args...);
}
