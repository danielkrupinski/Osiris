#pragma once

#include "Utils.h"

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

struct ModelRenderInfo {
    Vector                  origin;
    QAngle                  angles;
    char                    pad[4];
    ClientRenderable*       pRenderable;
    const Model*            pModel;
    const matrix3x4_t*      pModelToWorld;
    const matrix3x4_t*      pLightingOffset;
    const Vector*           pLightingOrigin;
    int                     flags;
    int                     entity_index;
    int                     skin;
    int                     body;
    int                     hitboxset;
    ModelInstanceHandle_t   instance;
};

class ModelRender {
public:
    void forceMaterialOverride(Material* newMaterial, int nOverrideType = 0, int nOverrides = 0)
    {
        callVirtualFunction<void(__thiscall*)(void*, Material*, int, int)>(this, 1)(this, newMaterial, nOverrideType, nOverrides);
    }
};
