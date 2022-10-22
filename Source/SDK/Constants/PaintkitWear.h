#pragma once

#include <limits>

namespace csgo::paintkit_wear
{

constexpr auto factoryNew = (std::numeric_limits<float>::min)();
constexpr auto minimalWear = 0.07f;
constexpr auto fieldTested = 0.15f;
constexpr auto wellWorn = 0.38f;
constexpr auto battleScarred = 0.45f;
constexpr auto maxWear = 1.0f;

}
