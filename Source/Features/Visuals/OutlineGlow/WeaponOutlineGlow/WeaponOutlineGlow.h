#pragma once

#include <Features/Visuals/OutlineGlow/OutlineGlowParams.h>
#include "WeaponOutlineGlowContext.h"

template <typename HookContext, typename Context = WeaponOutlineGlowContext<HookContext>>
class WeaponOutlineGlow {
public:
    template <typename... Args>
    WeaponOutlineGlow(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }
    
    void applyGlowToWeapon(EntityTypeInfo entityTypeInfo, auto&& weapon) const noexcept
    {
        auto&& condition = context.condition();
        if (!condition.shouldRun() || !condition.shouldGlowWeapon(weapon))
            return;

        using namespace outline_glow_params;
        const auto color = getColor(entityTypeInfo);
        weapon.applyGlow(color, kWeaponGlowRange);
        weapon.forEachChild([color](auto&& entity) { entity.applyGlow(color, kWeaponGlowRange); });
    }

private:
    [[nodiscard]] cs2::Color getColor(EntityTypeInfo entityTypeInfo) const noexcept
    {
        using namespace outline_glow_params;

        switch (entityTypeInfo.typeIndex) {
        case utils::typeIndex<cs2::C_MolotovGrenade, KnownEntityTypes>():
        case utils::typeIndex<cs2::C_IncendiaryGrenade, KnownEntityTypes>(): return kMolotovColor;
        case utils::typeIndex<cs2::C_Flashbang, KnownEntityTypes>(): return kFlashbangColor;
        case utils::typeIndex<cs2::C_HEGrenade, KnownEntityTypes>(): return kHEGrenadeColor;
        case utils::typeIndex<cs2::C_SmokeGrenade, KnownEntityTypes>(): return kSmokeGrenadeColor;
        default: return kDefaultWeaponColor;
        }
    }

    Context context;
};
