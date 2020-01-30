#pragma once

#include <cstddef>

struct WeaponData {
    std::byte pad1[4];
    char *name;
    std::byte pad1_[12];
    int maxClip;
    std::byte pad2[180];
    int type;
    std::byte pad2_[28];
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
    bool silencer;
    std::byte pad6[15];
    float maxSpeed;
    float maxSpeedAlt;
};
