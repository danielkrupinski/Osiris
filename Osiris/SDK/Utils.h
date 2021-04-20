#pragma once

#include <numbers>
#include <tuple>

[[deprecated]] constexpr auto radiansToDegrees = [](float radians) constexpr noexcept { return radians * 180.0f / std::numbers::pi_v<float>; };

std::tuple<float, float, float> rainbowColor(float speed) noexcept;
