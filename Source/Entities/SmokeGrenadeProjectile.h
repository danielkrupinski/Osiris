#pragma once

#include <CS2/Classes/Entities/GrenadeProjectiles.h>
#include <MemoryPatterns/PatternTypes/SmokeGrenadeProjectilePatternTypes.h>

template <typename HookContext>
class SmokeGrenadeProjectile {
public:
    SmokeGrenadeProjectile(HookContext& hookContext, cs2::C_SmokeGrenadeProjectile* smokeGrenadeProjectile) noexcept
        : hookContext{hookContext}
        , smokeGrenadeProjectile{smokeGrenadeProjectile}
    {
    }

    using RawType = cs2::C_SmokeGrenadeProjectile;

    [[nodiscard]] auto didSmokeEffect() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToDidSmokeEffect>().of(smokeGrenadeProjectile).toOptional();
    }

private:
    HookContext& hookContext;
    cs2::C_SmokeGrenadeProjectile* smokeGrenadeProjectile;
};
