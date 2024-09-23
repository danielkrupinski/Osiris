#pragma once

#include <GameClasses/OffsetTypes/SmokeGrenadeProjectileOffset.h>

struct SmokeGrenadeProjectileDeps {
    SmokeGrenadeProjectileDeps() = default;

    template <typename SmokeGrenadeProjectilePatterns>
    explicit SmokeGrenadeProjectileDeps(const SmokeGrenadeProjectilePatterns& smokeGrenadeProjectilePatterns) noexcept
        : offsetToDidSmokeEffect{smokeGrenadeProjectilePatterns.offsetToDidSmokeEffect()}
    {
    }

    OffsetToDidSmokeEffect offsetToDidSmokeEffect;
};
