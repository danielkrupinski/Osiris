#pragma once

#include <cassert>

#include "NormalizedDeviceCoordinates.h"

struct ClipSpaceCoordinates {
    [[nodiscard]] bool onScreen() const noexcept
    {
        return w >= 0.001f;
    }

    [[nodiscard]] NormalizedDeviceCoordinates toNormalizedDeviceCoordinates() const noexcept
    {
        assert(onScreen());
        const auto inverseW = 1.0f / w;
        return NormalizedDeviceCoordinates{ x * inverseW, y * inverseW };
    }

    float x;
    float y;
    float z;
    float w;
};
