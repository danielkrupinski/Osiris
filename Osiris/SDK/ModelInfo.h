#pragma once

#include <cstdint>

#include "Pad.h"
#include "Vector.h"
#include "VirtualMethod.h"

struct StudioBbox {
    int bone;
    int group;
    Vector bbMin;
    Vector bbMax;
    int hitboxNameIndex;
    Vector offsetOrientation;
    float capsuleRadius;
    int	unused[4];
};

struct StudioHitboxSet {
    int nameIndex;
    int numHitboxes;
    int hitboxIndex;

    const char* getName() noexcept
    {
        return nameIndex ? reinterpret_cast<const char*>(std::uintptr_t(this) + nameIndex) : nullptr;
    }

    StudioBbox* getHitbox(int i) noexcept
    {
        return i >= 0 && i < numHitboxes ? reinterpret_cast<StudioBbox*>(std::uintptr_t(this) + hitboxIndex) + i : nullptr;
    }
};

constexpr auto MAXSTUDIOBONES = 256;
constexpr auto BONE_USED_BY_HITBOX = 0x100;

struct StudioBone {
    int nameIndex;
    int	parent;
    PAD(152)
    int flags;
    PAD(52)

    const char* getName() const noexcept
    {
        return nameIndex ? reinterpret_cast<const char*>(std::uintptr_t(this) + nameIndex) : nullptr;
    }
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

    StudioHitboxSet* getHitboxSet(int i) noexcept
    {
        return i >= 0 && i < numHitboxSets ? reinterpret_cast<StudioHitboxSet*>(std::uintptr_t(this) + hitboxSetIndex) + i : nullptr;
    }
};

struct Model;

class ModelInfo {
public:
    VIRTUAL_METHOD(int, getModelIndex, 2, (const char* name), (this, name))
    VIRTUAL_METHOD(StudioHdr*, getStudioModel, 32, (const Model* model), (this, model))
};
