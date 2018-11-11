#pragma once

#include "Utils.h"

class RenderView {
public:
    void setColorModulation(float const* colors)
    {
        return callVirtualFunction<void(__thiscall*)(void*, float const*)>(this, 6)(this, colors);
    }
};
