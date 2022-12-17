#include <cmath>
#include <numbers>

#include "Utils.h"

std::tuple<float, float, float> rainbowColor(float time, float speed) noexcept
{
    constexpr float pi = std::numbers::pi_v<float>;
    return std::make_tuple(std::sin(speed * time) * 0.5f + 0.5f,
                           std::sin(speed * time + 2 * pi / 3) * 0.5f + 0.5f,
                           std::sin(speed * time + 4 * pi / 3) * 0.5f + 0.5f);
}
