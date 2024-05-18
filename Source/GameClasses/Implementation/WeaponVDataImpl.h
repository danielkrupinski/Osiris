#pragma once

#include <GameClasses/OffsetTypes/WeaponVDataOffset.h>

struct WeaponVDataImpl {
    template <typename WeaponVDataPatterns>
    explicit WeaponVDataImpl(const WeaponVDataPatterns& weaponVDataPatterns) noexcept
        : offsetToWeaponName{weaponVDataPatterns.offsetToWeaponName()}
    {
    }

    OffsetToWeaponName offsetToWeaponName;
};
