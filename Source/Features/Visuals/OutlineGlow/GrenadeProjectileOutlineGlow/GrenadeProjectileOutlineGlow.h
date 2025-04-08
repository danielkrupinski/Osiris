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

    void applyGlowToGrenadeProjectile(EntityTypeInfo entityTypeInfo, auto&& grenadeProjectile) const noexcept
    {
        if (enabled() && shouldGlowGrenadeProjectile(entityTypeInfo, grenadeProjectile))
            grenadeProjectile.applyGlowRecursively(getColor(entityTypeInfo));
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

    [[nodiscard]] cs2::Color getColor(EntityTypeInfo entityTypeInfo) const noexcept
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

    HookContext& hookContext;
};
