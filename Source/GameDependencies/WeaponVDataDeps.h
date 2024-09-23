#pragma once

#include <GameClasses/OffsetTypes/WeaponVDataOffset.h>

struct WeaponVDataDeps {
    WeaponVDataDeps() = default;

    template <typename WeaponVDataPatterns>
    explicit WeaponVDataDeps(const WeaponVDataPatterns& weaponVDataPatterns) noexcept
        : offsetToWeaponName{weaponVDataPatterns.offsetToWeaponName()}
    {
    }

    OffsetToWeaponName offsetToWeaponName;
};
