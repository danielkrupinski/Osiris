#pragma once

#include <vector>

class Entity;

struct Vector;

namespace Multipoints
{
    enum HitBoxes
    {
        HITBOX_HEAD,
        HITBOX_NECK,
        HITBOX_PELVIS,
        HITBOX_ABDOMEN,
        HITBOX_KIDNEYS,
        HITBOX_STERNUM,
        HITBOX_CLAVICLES,
        HITBOX_LEFT_THIGH,
        HITBOX_RIGHT_THIGH,
        HITBOX_LEFT_SHIN,
        HITBOX_RIGHT_SHIN,
        HITBOX_LEFT_ANKLE,
        HITBOX_RIGHT_ANKLE,
        HITBOX_LEFT_HAND,
        HITBOX_RIGHT_HAND,
        HITBOX_LEFT_ARM,
        HITBOX_LEFT_FOREARM,
        HITBOX_RIGHT_ARM,
        HITBOX_RIGHT_FOREARM,
        HITBOX_LAST_ENTRY = HITBOX_RIGHT_FOREARM,
        HITBOX_ALL,
        HITBOX_START = HITBOX_HEAD,
        HITBOX_MAX = HITBOX_ALL,
    };

    enum
    {
        MULTIPOINTS_CENTER,
        MULTIPOINTS_START = MULTIPOINTS_CENTER,
        MULTIPOINTS_ALL = 9,
        MULTIPOINTS_MAX = MULTIPOINTS_ALL,
    };

    static const char* hitBoxes[Multipoints::HITBOX_MAX] =
    {
        "Head",
        "Neck",
        "Pelvis",
        "Abdomen",
        "Kidneys",
        "Sternum",
        "Clavicles",
        "Left Thigh",
        "Right Thigh",
        "Left Shin",
        "Right Shin",
        "Left Ankle",
        "Right Ankle",
        "Left Hand",
        "Right Hand",
        "Left Arm",
        "Left Forearm",
        "Right Arm",
        "Right Forearm",
    };

    bool retrieveAll(Entity* entity, float multiPointsExpansion, Vector(&multiPoints)[Multipoints::HITBOX_MAX][Multipoints::MULTIPOINTS_MAX]);
    bool retrieveOne(Entity* entity, float multiPointsExpansion, Vector(&multiPoints)[Multipoints::MULTIPOINTS_MAX], int desiredHitBox = Multipoints::HITBOX_HEAD);
};