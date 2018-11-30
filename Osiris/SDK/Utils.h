#pragma once

#include <cstdint>

template<typename T>
constexpr auto callVirtualFunction(void* classBase, int index) noexcept
{
    return reinterpret_cast<T>((*reinterpret_cast<std::uintptr_t**>(classBase))[index]);
}
