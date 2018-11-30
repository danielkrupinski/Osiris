#pragma once

#include "Utils.h"

class RenderView {
public:
    constexpr void setBlend(float alpha) noexcept
    {
        return callVirtualFunction<void(__thiscall*)(void*, float)>(this, 4)(this, alpha);
    }

    constexpr void setColorModulation(const float* colors) noexcept
    {
        return callVirtualFunction<void(__thiscall*)(void*, const float*)>(this, 6)(this, colors);
    }
};
