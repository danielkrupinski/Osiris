#pragma once

#include <GameClasses/OffsetTypes/WeaponServicesOffset.h>

struct WeaponServicesDeps {
    template <typename WeaponServicesPatterns>
    explicit WeaponServicesDeps(const WeaponServicesPatterns& weaponServicesPatterns) noexcept
        : offsetToActiveWeapon{weaponServicesPatterns.offsetToActiveWeapon()}
    {
    }

    OffsetToActiveWeapon offsetToActiveWeapon;
};
