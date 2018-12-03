#pragma once

#include "Material.h"
#include "Utils.h"

class ModelRender {
public:
    void forceMaterialOverride(Material* newMaterial)
    {
        callVirtualFunction<void(__thiscall*)(void*, Material*, int, int)>(this, 1)(this, newMaterial, 0, 0);
    }
};
