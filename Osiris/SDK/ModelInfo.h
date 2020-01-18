#pragma once

#include "Utils.h"
#include "Vector.h"

struct StudioHitboxSet {
    int nameIndex;
    int numHitboxes;
    int hitboxIndex;
};

struct StudioHdr {
    int id;
    int version;
    int checksum;
    char name[64];
    int length;
    Vector eyePosition;
    Vector illumPosition;
    Vector hullMin;
    Vector hullMax;
    Vector bbMin;
    Vector bbMax;
    int flags;
    int numBones;
    int boneIndex;
    int numBoneControllers;
    int boneControllerIndex;
    int numHitboxSets;
    int hitboxSetIndex;
};

struct Model;

class ModelInfo {
public:
    constexpr auto getModelIndex(const char* name) noexcept
    {
        return callVirtualMethod<int, const char*>(this, 2, name);
    }

    constexpr auto getStudioModel(const Model* model) noexcept
    {
        return callVirtualMethod<StudioHdr*, const Model*>(this, 32, model);
    }
};
