#pragma once

#include <utility>

#include <CS2/Classes/Entities/WeaponEntities.h>
#include <GameClient/Entities/EntityClassifier.h>
#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>

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

    [[nodiscard]] cs2::Color getGlowColor(EntityTypeInfo entityTypeInfo, auto&& /* weapon */) const noexcept
    {
        using namespace outline_glow_params;

        switch (entityTypeInfo.typeIndex) {
        case EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>():
        case EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>(): return kMolotovColor;
        case EntityTypeInfo::indexOf<cs2::C_Flashbang>(): return kFlashbangColor;
        case EntityTypeInfo::indexOf<cs2::C_HEGrenade>(): return kHEGrenadeColor;
        case EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>(): return kSmokeGrenadeColor;
        default: return kDefaultWeaponColor;
        }
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

    HookContext& hookContext;
};
