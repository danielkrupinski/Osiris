#pragma once

#include "Material.h"
#include "Utils.h"
#include "Vector.h"

using matrix3x4 = float[3][4];

struct Model {
    void* handle;
    char name[260];
    int loadFlags;
    int serverCount;
    int type;
    int flags;
    Vector  vecMins;
    Vector  vecMaxs;
    float   radius;
};

struct ModelRenderInfo {
    Vector origin;
    Vector angles;
    char pad[4];
    void* renderable;
    const Model* model;
    const matrix3x4* modelToWorld;
    const matrix3x4* lightingOffset;
    const Vector* lightingOrigin;
    int flags;
    int entityIndex;
    int skin;
    int body;
    int hitboxset;
    // ModelInstanceHandle_t   instance;
};

class ModelRender {
public:
    constexpr void forceMaterialOverride(Material* newMaterial) noexcept
    {
        callVirtualFunction<void(__thiscall*)(void*, Material*, int, int)>(this, 1)(this, newMaterial, 0, 0);
    }

    constexpr bool isMaterialOverriden() noexcept
    {
        return callVirtualFunction<bool(__thiscall*)(void*)>(this, 2)(this);
    }
};
