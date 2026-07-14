#pragma once

#include <utility>

#include <GameClient/Entities/BaseWeapon.h>
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

    [[nodiscard]] bool shouldApplyGlow(auto&& weapon) const noexcept
    {
        return !weapon.baseEntity().hasOwner().valueOr(true);
    }

    [[nodiscard]] Optional<color::HueInteger> hue(auto&& weapon) const
    {
        switch (weapon.grenadeKind().valueOr(cs2::GrenadeKind::None)) {
        case cs2::GrenadeKind::Molotov:
        case cs2::GrenadeKind::Incendiary: return GET_CONFIG_VAR(outline_glow_vars::MolotovHue);
        case cs2::GrenadeKind::Flashbang: return GET_CONFIG_VAR(outline_glow_vars::FlashbangHue);
        case cs2::GrenadeKind::HEGrenade: return GET_CONFIG_VAR(outline_glow_vars::HEGrenadeHue);
        case cs2::GrenadeKind::SmokeGrenade: return GET_CONFIG_VAR(outline_glow_vars::SmokeGrenadeHue);
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
