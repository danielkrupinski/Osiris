#pragma once

#include "Utils.h"

class RenderView {
public:
    constexpr void setBlend(float alpha) noexcept
    {
        return callVirtualMethod<void, float>(this, 4, alpha);
    }

    constexpr void setColorModulation(const float* colors) noexcept
    {
        return callVirtualMethod<void, const float*>(this, 6, colors);
    }

    constexpr void setColorModulation(float r, float g, float b) noexcept
    {
        float color[3]{ r, g, b };
        return callVirtualMethod<void, const float*>(this, 6, color);
    }
};
