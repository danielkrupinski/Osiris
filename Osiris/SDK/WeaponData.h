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
    PAD(IS_WIN32() ? 20 : 32)
    int maxClip;
    PAD(IS_WIN32() ? 112 : 204)
    const char* name;
    PAD(IS_WIN32() ? 60 : 72)
    WeaponType type;
    PAD(4)
    int price;
    PAD(IS_WIN32() ? 8 : 12)
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
    PAD(16)
    bool silencer;
    PAD(IS_WIN32() ? 15 : 23)
    float maxSpeed;
    float maxSpeedAlt;
    PAD(100)
    float recoilMagnitude;
    float recoilMagnitudeAlt;
    PAD(16)
    float recoveryTimeStand;
};
