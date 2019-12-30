#pragma once

#include <cstddef>

struct WeaponData {
    std::byte pad[20];
    int maxClip;
    std::byte pad1[112];
    char* name;
    std::byte pad1_[60];
    int type;
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
    bool silencer;
    std::byte pad6[15];
    float maxSpeed;
    float maxSpeedAlt;
};
