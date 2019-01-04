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

    void setMaterialVarFlag(MaterialVar flag, bool isOn)
    {
        callVirtualFunction<void(__thiscall*)(void*, MaterialVar, bool)>(this, 29)(this, flag, isOn);
    }
};
