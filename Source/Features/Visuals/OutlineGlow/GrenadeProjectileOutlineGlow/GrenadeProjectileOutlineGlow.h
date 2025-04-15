#pragma once

#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Entities/GrenadeProjectiles.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <GameClient/Entities/SmokeGrenadeProjectile.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>

template <typename HookContext>
class GrenadeProjectileOutlineGlow {
public:
    explicit GrenadeProjectileOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo entityTypeInfo, auto&& grenadeProjectile) const noexcept
    {
        return enabled() && shouldGlowGrenadeProjectile(entityTypeInfo, grenadeProjectile);
    }

    [[nodiscard]] cs2::Color getGlowColor(EntityTypeInfo entityTypeInfo, auto&& /* grenadeProjectile */) const noexcept
    {
        using namespace outline_glow_params;

        switch (entityTypeInfo.typeIndex) {
        case EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>(): return kFlashbangColor;
        case EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>(): return kHEGrenadeColor;
        case EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>(): return kMolotovColor;
        case EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>(): return kSmokeGrenadeColor;
        default: return kDefaultWeaponColor;
        }
    }

private:
    [[nodiscard]] bool enabled() const noexcept
    {
        return hookContext.config().template getVariable<GrenadeProjectileOutlineGlowEnabled>();
    }

    [[nodiscard]] bool shouldGlowGrenadeProjectile(EntityTypeInfo entityTypeInfo, auto&& grenadeProjectile) const noexcept
    {
        return !entityTypeInfo.is<cs2::C_SmokeGrenadeProjectile>() || !grenadeProjectile.template as<SmokeGrenadeProjectile>().didSmokeEffect().valueOr(false);
    }

    HookContext& hookContext;
};
