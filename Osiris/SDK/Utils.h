#pragma once

#include <cmath>
#include <cstdint>

template<typename T, typename ...Args>
constexpr auto callVirtualMethod(void* classBase, int index, Args... args) noexcept
{
    return ((*reinterpret_cast<T(__thiscall***)(void*, Args...)>(classBase))[index])(classBase, args...);
}

constexpr auto degreesToRadians = [](float degrees) constexpr noexcept { return degrees * static_cast<float>(M_PI) / 180.0f; };
constexpr auto radiansToDegrees = [](float radians) constexpr noexcept { return radians * 180.0f / static_cast<float>(M_PI); };
