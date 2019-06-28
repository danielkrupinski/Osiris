#pragma once

#include <cstddef>

struct WeaponData {
    std::byte pad[200];
    int type;
    std::byte pad1[32];
    bool fullAuto;
    std::byte pad2[3];
    int damage;
    float armorRatio;
    int bullets;
    float penetration;
    std::byte pad3[8];
    float range;
    float rangeModifier;
    std::byte pad4[16];
    bool hasSilencer;
};
