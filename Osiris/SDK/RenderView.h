#pragma once

#include "VirtualMethod.h"

class RenderView {
public:
    VIRTUAL_METHOD(void, setBlend, 4, (float alpha), (this, alpha))
    VIRTUAL_METHOD(void, setColorModulation, 6, (const float* colors), (this, colors))

    constexpr void setColorModulation(float r, float g, float b) noexcept
    {
        float color[3]{ r, g, b };
        setColorModulation(color);
    }
};
