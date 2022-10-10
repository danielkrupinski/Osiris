#pragma once

#include "VirtualMethod.h"

enum class WeaponId : short;
struct WeaponInfo;

class WeaponSystem : private VirtualCallable {
public:
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD2(WeaponInfo*, getWeaponInfo, 2, (WeaponId weaponId), (weaponId))
};
