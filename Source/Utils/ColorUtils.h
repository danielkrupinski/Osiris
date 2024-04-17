#pragma once

#include <cstdint>

#include <CS2/Classes/Color.h>

#include "InRange.h"

namespace color
{

using Hue = InRange<float, 0.0f, 1.0f>;
using Saturation = InRange<float, 0.0f, 1.0f>;
using Brightness = InRange<float, 0.0f, 1.0f>;

constexpr Hue kGreenHue = 120.0f / 360.0f;
constexpr Hue kRedHue = 0.0f / 360.0f;

[[nodiscard]] constexpr cs2::Color HSBtoRGB(Hue hue, Saturation saturation, Brightness brightness) noexcept
{
    constexpr auto kNumberOfHueSegments = 6;

    const auto huePosition = hue * kNumberOfHueSegments;
    const auto hueSegment = static_cast<int>(huePosition);
    const auto hueFractionInSegment = huePosition - hueSegment;

    const auto chroma = brightness * saturation;
    const auto max = brightness;
    const auto min = max - chroma;
    const auto midDescending = max - chroma * hueFractionInSegment;
    const auto midAscending = min + chroma * hueFractionInSegment;

    float r, g, b;
    switch (hueSegment) {
    case 0: r = max; g = midAscending; b = min; break;
    case 1: r = midDescending; g = max; b = min; break;
    case 2: r = min; g = max; b = midAscending; break;
    case 3: r = min; g = midDescending; b = max; break;
    case 4: r = midAscending; g = min; b = max; break;
    case 5: default: r = max; g = min; b = midDescending; break;
    }

    return cs2::Color{static_cast<std::uint8_t>(r * 255), static_cast<std::uint8_t>(g * 255), static_cast<std::uint8_t>(b * 255)};
}

}
