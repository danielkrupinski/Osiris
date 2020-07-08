#pragma once

#include "Utils.h"
#include "Vector.h"

class matrix3x4;

struct Model {
    void* handle;
    char name[260];
    int	loadFlags;
    int	serverCount;
    int	type;
    int	flags;
    Vector mins, maxs;
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
