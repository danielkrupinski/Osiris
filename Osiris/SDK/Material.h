#pragma once

#include "Utils.h"

enum class MaterialVar {
    NO_DRAW = 1 << 2,
    IGNOREZ = 1 << 15,
    WIREFRAME = 1 << 28,
};

class Material {
public:
    void incrementReferenceCount()
    {
        callVirtualFunction<void(__thiscall*)(void*)>(this, 12)(this);
    }

    void alphaModulate(float alpha)
    {
        callVirtualFunction<void(__thiscall*)(void*, float)>(this, 27)(this, alpha);
    }

    void colorModulate(float color[3])
    {
        callVirtualFunction<void(__thiscall*)(void*, float, float, float)>(this, 28)(this, color[0], color[1], color[2]);
    }

    void setMaterialVarFlag(MaterialVar flag, bool isOn)
    {
        callVirtualFunction<void(__thiscall*)(void*, MaterialVar, bool)>(this, 29)(this, flag, isOn);
    }
};
