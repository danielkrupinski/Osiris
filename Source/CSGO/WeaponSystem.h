#pragma once

#include "VirtualMethod.h"

enum class WeaponId : short;

namespace csgo
{

struct WeaponInfo;
struct WeaponSystemPOD;

struct WeaponSystem : GameClass<WeaponSystem, WeaponSystemPOD> {
    VIRTUAL_METHOD(WeaponInfo*, getWeaponInfo, 2, (WeaponId weaponId), (weaponId))
};

}
