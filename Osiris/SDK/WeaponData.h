#pragma once

#include "Pad.h"

enum class WeaponType {
    Knife = 0,
    Pistol,
    SubMachinegun,
    Rifle,
    Shotgun,
    SniperRifle,
    Machinegun,
    C4,
    Placeholder,
    Grenade,
    Unknown
};

struct WeaponInfo {
    PAD(20)
    int maxClip;
    PAD(112)
    char* name;
    PAD(60)
    WeaponType type;
    PAD(4)
    int price;
    PAD(0x8)
    float cycletime;
    PAD(12)
    bool fullAuto;
    PAD(3)
    int damage;
    float armorRatio;
    int bullets;
    float penetration;
    PAD(8)
    float range;
    float rangeModifier;
    PAD(0x10)
    bool silencer;
    PAD(15)
    float maxSpeed;
    float maxSpeedAlt;
    PAD(100)
    float recoilMagnitude;
    float recoilMagnitudeAlt;
    PAD(0x14)
    float recoveryTimeStand;
};
