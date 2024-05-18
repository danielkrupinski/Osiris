#pragma once

#include <GameClasses/OffsetTypes/WeaponServicesOffset.h>

struct WeaponServicesImpl {
    template <typename WeaponServicesPatterns>
    explicit WeaponServicesImpl(const WeaponServicesPatterns& weaponServicesPatterns) noexcept
        : offsetToActiveWeapon{weaponServicesPatterns.offsetToActiveWeapon()}
    {
    }

    OffsetToActiveWeapon offsetToActiveWeapon;
};
