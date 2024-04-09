#pragma once

#include <MemoryPatterns/WeaponServicesPatterns.h>

struct WeaponServicesImpl {
    explicit WeaponServicesImpl(const WeaponServicesPatterns& weaponServicesPatterns) noexcept
        : offsetToActiveWeapon{weaponServicesPatterns.offsetToActiveWeapon()}
    {
    }

    OffsetToActiveWeapon offsetToActiveWeapon;
};
