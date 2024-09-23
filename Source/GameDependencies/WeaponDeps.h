#pragma once

#include <GameClasses/OffsetTypes/WeaponOffset.h>

struct WeaponDeps {
    WeaponDeps() = default;

    template <typename WeaponPatterns>
    explicit WeaponDeps(const WeaponPatterns& weaponPatterns) noexcept
        : offsetToClipAmmo{weaponPatterns.offsetToClipAmmo()}
    {
    }

    OffsetToClipAmmo offsetToClipAmmo;
};
