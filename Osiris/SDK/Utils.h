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

constexpr auto rainbowColor(float time, float speed) noexcept
{
    return std::make_tuple(std::sin(speed * time) * 0.5f + 0.5f,
                           std::sin(speed * time + 2.0f) * 0.5f + 0.5f,
                           std::sin(speed * time + 4.0f) * 0.5f + 0.5f);
}

constexpr auto rainbowColor2(float time, float speed, const float spectrum[3]) noexcept
{
    return std::make_tuple(std::sin(speed * time + spectrum[0]) * 0.5f + 0.5f,
                           std::sin(speed * time + spectrum[1]) * 0.5f + 0.5f,
                           std::sin(speed * time + spectrum[2]) * 0.5f + 0.5f);
}
