#pragma once

#include "Utils.h"
#include "Vector.h"

using matrix3x4 = float[3][4];

struct Model {
    void* handle;
    char name[260];
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
};

class Material;

class ModelRender {
public:
    constexpr void forceMaterialOverride(Material* newMaterial) noexcept
    {
        callVirtualMethod<void, Material*, int, int>(this, 1, newMaterial, 0, 0);
    }

    constexpr bool isMaterialOverriden() noexcept
    {
        return callVirtualMethod<bool>(this, 2);
    }
};
