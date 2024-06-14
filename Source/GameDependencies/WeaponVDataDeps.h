#pragma once

#include <GameClasses/OffsetTypes/WeaponVDataOffset.h>

struct WeaponVDataDeps {
    template <typename WeaponVDataPatterns>
    explicit WeaponVDataDeps(const WeaponVDataPatterns& weaponVDataPatterns) noexcept
        : offsetToWeaponName{weaponVDataPatterns.offsetToWeaponName()}
    {
    }

    OffsetToWeaponName offsetToWeaponName;
};
