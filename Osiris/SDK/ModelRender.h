#pragma once

#include "Utils.h"

struct ModelRenderInfo {
    Vector                  origin;
    QAngle                  angles;
    char                    pad[4];
    ClientRenderable*      pRenderable;
    const model_t*          pModel;
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
