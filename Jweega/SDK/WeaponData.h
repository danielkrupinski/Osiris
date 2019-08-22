#pragma once

#include <cstddef>
#include "WeaponType.h"

struct WeaponData {
    std::byte pad[20];
    int maxClip;
    std::byte pad1[176];
    WeaponType type;
    std::byte pad2[32];
    bool fullAuto;
    std::byte pad3[3];
    int damage;
    float armorRatio;
    int bullets;
    float penetration;
    std::byte pad4[8];
    float range;
    float rangeModifier;
    std::byte pad5[16];
    bool hasSilencer;
};
