#pragma once

#include <utility>

#include <CS2/Classes/Entities/WeaponEntities.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include <Utils/ColorUtils.h>

template <typename HookContext>
class WeaponOutlineGlow {
public:
    explicit WeaponOutlineGlow(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] bool shouldApplyGlow(EntityTypeInfo /* entityTypeInfo */, auto&& weapon) const noexcept
    {
        return enabled() && shouldGlowWeapon(weapon);
    }

    [[nodiscard]] Optional<color::Hue> getGlowHue(EntityTypeInfo entityTypeInfo, auto&& /* weapon */) const noexcept
    {
        if (const auto hue = getGlowColorHue(entityTypeInfo); hue.hasValue())
            return hue.value().toHueFloat();
        return {};
    }

    [[nodiscard]] int getGlowRange() const noexcept
    {
        return outline_glow_params::kWeaponGlowRange;
    }

private:
    [[nodiscard]] bool enabled() const noexcept
    {
        return hookContext.config().template getVariable<WeaponOutlineGlowEnabled>();
    }

    [[nodiscard]] bool shouldGlowWeapon(auto&& weapon) const noexcept
    {
        return !weapon.hasOwner().valueOr(true);
    }

    [[nodiscard]] Optional<color::HueInteger> getGlowColorHue(EntityTypeInfo entityTypeInfo) const noexcept
    {
        using namespace outline_glow_params;

        switch (entityTypeInfo.typeIndex) {
        case EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>():
        case EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>(): return kMolotovHue;
        case EntityTypeInfo::indexOf<cs2::C_Flashbang>(): return kFlashbangHue;
        case EntityTypeInfo::indexOf<cs2::C_HEGrenade>(): return kHEGrenadeHue;
        case EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>(): return kSmokeGrenadeHue;
        default: return {};
        }
    }

    HookContext& hookContext;
};
