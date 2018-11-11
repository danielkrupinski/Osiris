#pragma once

#include "Utils.h"

class RenderView {
    void setColorModulation(const float* colors)
    {
        return callVirtualFunction<void(__thiscall*)(void*, const float*)>(this, 6)(this, colors);
    }
};
