#pragma once

#include "Utils.h"
#include "Vector.h"
#include "QAngle.h"
#include "ClientRenderable.h"
#include "Material.h"

struct Model {
    void*   fnHandle;
    char    szName[260];
    __int32 nLoadFlags;
    __int32 nServerCount;
    __int32 type;
    __int32 flags;
    Vector  vecMins;
    Vector  vecMaxs;
    float   radius;
    char    pad[0x1C];
};

class ModelRender {
public:
    void forceMaterialOverride(Material* newMaterial, int nOverrideType = 0, int nOverrides = 0)
    {
        callVirtualFunction<void(__thiscall*)(void*, Material*, int, int)>(this, 1)(this, newMaterial, nOverrideType, nOverrides);
    }
};
