#pragma once

#include <cmath>
#include <cstdint>
#include <tuple>

constexpr auto degreesToRadians = [](float degrees) constexpr noexcept { return degrees * static_cast<float>(M_PI) / 180.0f; };
constexpr auto radiansToDegrees = [](float radians) constexpr noexcept { return radians * 180.0f / static_cast<float>(M_PI); };

constexpr auto rainbowColor(float time, float speed) noexcept
{
    return std::make_tuple(std::sin(speed * time) * 0.5f + 0.5f,
                           std::sin(speed * time + static_cast<float>(2 * M_PI / 3)) * 0.5f + 0.5f,
                           std::sin(speed * time + static_cast<float>(4 * M_PI / 3)) * 0.5f + 0.5f);
}
