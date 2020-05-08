#pragma once

#include "VirtualMethod.h"

enum class WeaponId : short;
struct WeaponInfo;

class WeaponSystem {
public:
    VIRTUAL_METHOD(WeaponInfo*, getWeaponInfo, 2, (WeaponId weaponId), (this, weaponId))
};
