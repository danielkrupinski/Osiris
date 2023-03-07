#pragma once

#include "VirtualMethod.h"

enum class WeaponId : short;

namespace csgo
{

struct WeaponInfo;

struct WeaponSystem : private VirtualCallable {
    using VirtualCallable::VirtualCallable;

    VIRTUAL_METHOD(WeaponInfo*, getWeaponInfo, 2, (WeaponId weaponId), (weaponId))
};

}
