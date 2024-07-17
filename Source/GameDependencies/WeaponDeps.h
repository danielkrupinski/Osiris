#pragma once

#include <GameClasses/OffsetTypes/WeaponOffset.h>

struct WeaponDeps {
    template <typename WeaponPatterns>
    explicit WeaponDeps(const WeaponPatterns& weaponPatterns) noexcept
        : offsetToClipAmmo{weaponPatterns.offsetToClipAmmo()}
    {
    }

    OffsetToClipAmmo offsetToClipAmmo;
};
