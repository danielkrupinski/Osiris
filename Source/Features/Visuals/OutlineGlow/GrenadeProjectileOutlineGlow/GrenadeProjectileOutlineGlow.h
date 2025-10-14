#pragma once

#include <utility>

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Entities/GrenadeProjectiles.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <GameClient/Entities/SmokeGrenadeProjectile.h>
#include <HookContext/HookContextMacros.h>
#include <Utils/Optional.h>

template <typename HookContext>
class GrenadeProjectileOutlineGlow {
public:
    explicit GrenadeProjectileOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool enabled() const noexcept
    {
        return GET_CONFIG_VAR(outline_glow_vars::GlowGrenadeProjectiles);
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo entityTypeInfo, auto&& grenadeProjectile) const noexcept
    {
        return !entityTypeInfo.is<cs2::C_SmokeGrenadeProjectile>() || !grenadeProjectile.template as<SmokeGrenadeProjectile>().didSmokeEffect().valueOr(false);
    }

    [[nodiscard]] Optional<color::HueInteger> hue(EntityTypeInfo entityTypeInfo, auto&& /* grenadeProjectile */) const noexcept
    {
        switch (entityTypeInfo.typeIndex) {
        case EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>(): return GET_CONFIG_VAR(outline_glow_vars::FlashbangHue);
        case EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>(): return GET_CONFIG_VAR(outline_glow_vars::HEGrenadeHue);
        case EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>(): return GET_CONFIG_VAR(outline_glow_vars::MolotovHue);
        case EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>(): return GET_CONFIG_VAR(outline_glow_vars::SmokeGrenadeHue);
        default: return {};
        }
    }

private:
    HookContext& hookContext;
};
