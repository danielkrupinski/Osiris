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
    Unknown,
    StackableItem,
    Fists,
    BreachCharge,
    BumpMine,
    Tablet,
    Melee
};

struct WeaponInfo {
    PAD(WIN32_LINUX(20, 32))
    int maxClip;
    PAD(WIN32_LINUX(112, 204))
    const char* name;
    PAD(WIN32_LINUX(60, 72))
    WeaponType type;
    PAD(4)
    int price;
    PAD(WIN32_LINUX(8, 12))
    float cycletime;
    PAD(12)
    bool fullAuto;
    PAD(3)
    int damage;
    float headshotMultiplier;
    float armorRatio;
    int bullets;
    float penetration;
    PAD(8)
    float range;
    float rangeModifier;
    PAD(16)
    bool silencer;
    PAD(WIN32_LINUX(15, 23))
    float maxSpeed;
    float maxSpeedAlt;
    PAD(100)
    float recoilMagnitude;
    float recoilMagnitudeAlt;
    PAD(16)
    float recoveryTimeStand;
};
