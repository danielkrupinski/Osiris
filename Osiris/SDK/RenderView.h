#pragma once

#include "Utils.h"

class RenderView {
public:
    constexpr void setBlend(float alpha) noexcept
    {
        return callVirtualFunction<void, float>(this, 4, alpha);
    }

    constexpr void setColorModulation(const float* colors) noexcept
    {
        return callVirtualFunction<void, const float*>(this, 6, colors);
    }
};
