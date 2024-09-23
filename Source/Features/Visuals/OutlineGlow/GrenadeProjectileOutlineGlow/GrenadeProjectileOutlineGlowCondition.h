#pragma once

#include <utility>

#include <FeatureHelpers/EntityClassifier.h>
#include <GameClasses/SmokeGrenadeProjectile.h>

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
        return context.state().enabled;
    }

    [[nodiscard]] bool shouldGlowGrenadeProjectile(EntityTypeInfo entityTypeInfo, auto&& grenadeProjectile) const noexcept
    {
        return !entityTypeInfo.is<cs2::C_SmokeGrenadeProjectile>() || !grenadeProjectile.template as<SmokeGrenadeProjectile>().didSmokeEffect().valueOr(false);
    }

private:
    Context context;
};
