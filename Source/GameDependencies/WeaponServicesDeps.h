#pragma once

#include <GameClasses/OffsetTypes/WeaponServicesOffset.h>

struct WeaponServicesDeps {
    WeaponServicesDeps() = default;

    template <typename WeaponServicesPatterns>
    explicit WeaponServicesDeps(const WeaponServicesPatterns& weaponServicesPatterns) noexcept
        : offsetToActiveWeapon{weaponServicesPatterns.offsetToActiveWeapon()}
        , offsetToWeapons{weaponServicesPatterns.offsetToWeapons()}
    {
    }

    OffsetToActiveWeapon offsetToActiveWeapon;
    OffsetToWeapons offsetToWeapons;
};
