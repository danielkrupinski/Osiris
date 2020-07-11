#pragma once

struct ColorA;

namespace Helpers
{
    unsigned int calculateColor(ColorA color) noexcept;

    constexpr auto units2meters(float units) noexcept
    {
        return units * 0.0254f;
    }
}
