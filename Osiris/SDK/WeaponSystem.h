#pragma once

#include "Inconstructible.h"
#include "VirtualMethod.h"

enum class WeaponId : short;
struct WeaponInfo;

class WeaponSystem {
public:
    INCONSTRUCTIBLE(WeaponSystem)

    VIRTUAL_METHOD(WeaponInfo*, getWeaponInfo, 2, (WeaponId weaponId), (this, weaponId))
};
