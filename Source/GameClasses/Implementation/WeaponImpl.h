#pragma once

#include <GameClasses/OffsetTypes/WeaponOffset.h>

struct WeaponImpl {
    template <typename WeaponPatterns>
    explicit WeaponImpl(const WeaponPatterns& weaponPatterns) noexcept
        : offsetToClipAmmo{weaponPatterns.offsetToClipAmmo()}
    {
    }

    OffsetToClipAmmo offsetToClipAmmo;
};
