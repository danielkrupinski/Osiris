#pragma once

#include <cstdint>

#include <Utils/Pad.h>
#include "Vector.h"
#include "VirtualMethod.h"

enum class Hitbox {
    Head,
    Neck,
    Pelvis,
    Belly,
    Thorax,
    LowerChest,
    UpperChest,
    RightThigh,
    LeftThigh,
    RightCalf,
    LeftCalf,
    RightFoot,
    LeftFoot,
    RightHand,
    LeftHand,
    RightUpperArm,
    RightForearm,
    LeftUpperArm,
    LeftForearm,
    Max
};

constexpr auto MAXSTUDIOBONES = 256;
constexpr auto BONE_USED_BY_HITBOX = 0x100;

namespace csgo
{

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

    StudioBbox* getHitbox(Hitbox i) noexcept
    {
        return static_cast<int>(i) < numHitboxes ? reinterpret_cast<StudioBbox*>(std::uintptr_t(this) + hitboxIndex) + static_cast<int>(i) : nullptr;
    }
};

struct StudioBone {
    int nameIndex;
    int	parent;
    PAD(152);
    int flags;
    PAD(52);

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

    const StudioBone* getBone(int i) const noexcept
    {
        return i >= 0 && i < numBones ? reinterpret_cast<StudioBone*>(std::uintptr_t(this) + boneIndex) + i : nullptr;
    }

    StudioHitboxSet* getHitboxSet(int i) noexcept
    {
        return i >= 0 && i < numHitboxSets ? reinterpret_cast<StudioHitboxSet*>(std::uintptr_t(this) + hitboxSetIndex) + i : nullptr;
    }
};

struct Model;
struct ModelInfoPOD;

struct ModelInfo : GameClass<ModelInfo, ModelInfoPOD> {
    VIRTUAL_METHOD(int, getModelIndex, WIN32_LINUX(2, 3), (const char* name), (name))
    VIRTUAL_METHOD(StudioHdr*, getStudioModel, WIN32_LINUX(32, 31), (const Model* model), (model))
};

}
