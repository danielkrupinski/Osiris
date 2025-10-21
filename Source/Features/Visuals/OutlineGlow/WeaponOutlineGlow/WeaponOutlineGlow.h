#pragma once

#include <utility>

#include <CS2/Classes/Entities/WeaponEntities.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include <HookContext/HookContextMacros.h>
#include <Utils/ColorUtils.h>

template <typename HookContext>
class WeaponOutlineGlow {
public:
    explicit WeaponOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool enabled() const
    {
        return GET_CONFIG_VAR(outline_glow_vars::GlowWeapons);
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo /* entityTypeInfo */, auto&& weapon) const noexcept
    {
        return !weapon.hasOwner().valueOr(true);
    }

    [[nodiscard]] Optional<color::HueInteger> hue(EntityTypeInfo entityTypeInfo, auto&& /* weapon */) const
    {
        switch (entityTypeInfo.typeIndex) {
        case EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>():
        case EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>(): return GET_CONFIG_VAR(outline_glow_vars::MolotovHue);
        case EntityTypeInfo::indexOf<cs2::C_Flashbang>(): return GET_CONFIG_VAR(outline_glow_vars::FlashbangHue);
        case EntityTypeInfo::indexOf<cs2::C_HEGrenade>(): return GET_CONFIG_VAR(outline_glow_vars::HEGrenadeHue);
        case EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>(): return GET_CONFIG_VAR(outline_glow_vars::SmokeGrenadeHue);
        default: return {};
        }
    }

    [[nodiscard]] int getGlowRange() const noexcept
    {
        return outline_glow_params::kWeaponGlowRange;
    }

private:
    HookContext& hookContext;
};
