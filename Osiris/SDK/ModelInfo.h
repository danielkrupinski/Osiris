#pragma once

#include "Utils.h"
#include "Vector.h"

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

class ModelInfo {
public:
    constexpr auto getModelIndex(const char* name) noexcept
    {
        return callVirtualMethod<int, const char*>(this, 2, name);
    }
};
