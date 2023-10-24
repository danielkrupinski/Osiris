#pragma once

#include <cassert>

#include <CS2/Classes/Panorama.h>

struct NormalizedDeviceCoordinates {
    NormalizedDeviceCoordinates(float x, float y) noexcept
        : x{ x }, y{ y }
    {
        assert(x >= -1.0f && x <= 1.0f);
        assert(y >= -1.0f && y <= 1.0f);
    }

    [[nodiscard]] cs2::CUILength getX() const noexcept
    {
        return cs2::CUILength{ (x + 1.0f) * 0.5f * 100.0f, cs2::CUILength::k_EUILengthPercent };
    }

    [[nodiscard]] cs2::CUILength getY() const noexcept
    {
        return cs2::CUILength{ (1.0f - y) * 0.5f * 100.0f, cs2::CUILength::k_EUILengthPercent };
    }

private:
    float x;
    float y;
};
