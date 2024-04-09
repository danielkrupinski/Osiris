#pragma once

#include <MemoryPatterns/WeaponVDataPatterns.h>

struct WeaponVDataImpl {
    explicit WeaponVDataImpl(const WeaponVDataPatterns& weaponVDataPatterns) noexcept
        : offsetToWeaponName{weaponVDataPatterns.offsetToWeaponName()}
    {
    }

    OffsetToWeaponName offsetToWeaponName;
};
