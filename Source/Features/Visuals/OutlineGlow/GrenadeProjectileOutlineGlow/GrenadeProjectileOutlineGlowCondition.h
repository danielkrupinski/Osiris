#pragma once

#include <utility>

#include <CS2/Classes/Entities/GrenadeProjectiles.h>
#include <GameClient/Entities/SmokeGrenadeProjectile.h>
#include <GameClient/Entities/EntityClassifier.h>

template <typename Context>
class GrenadeProjectileOutlineGlowCondition {
public:
    template <typename... Args>
    explicit GrenadeProjectileOutlineGlowCondition(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] bool shouldRun() const noexcept
    {
        return context.config().template getVariable<GrenadeProjectileOutlineGlowEnabled>();
    }

    [[nodiscard]] bool shouldGlowGrenadeProjectile(EntityTypeInfo entityTypeInfo, auto&& grenadeProjectile) const noexcept
    {
        return !entityTypeInfo.is<cs2::C_SmokeGrenadeProjectile>() || !grenadeProjectile.template as<SmokeGrenadeProjectile>().didSmokeEffect().valueOr(false);
    }

private:
    Context context;
};
