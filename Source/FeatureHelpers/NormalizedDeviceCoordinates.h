#pragma once

#include <CS2/Panorama/CUILength.h>

struct NormalizedDeviceCoordinates {
    NormalizedDeviceCoordinates(float x, float y) noexcept
        : x{ x }, y{ y }
    {
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
