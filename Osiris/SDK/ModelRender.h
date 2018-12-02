#pragma once

#include "Utils.h"
#include "Vector.h"
#include "QAngle.h"
#include "ClientRenderable.h"
#include "Material.h"

class ModelRender {
public:
    void forceMaterialOverride(Material* newMaterial, int nOverrideType = 0, int nOverrides = 0)
    {
        callVirtualFunction<void(__thiscall*)(void*, Material*, int, int)>(this, 1)(this, newMaterial, nOverrideType, nOverrides);
    }
};
