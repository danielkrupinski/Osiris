#pragma once

#include <utility>

#include <CS2/Classes/Entities/WeaponEntities.h>
#include <GameClient/Entities/EntityClassifier.h>
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
        case EntityTypeInfo::indexOf<cs2::C_MolotovGrenade>():
        case EntityTypeInfo::indexOf<cs2::C_IncendiaryGrenade>(): return kMolotovColor;
        case EntityTypeInfo::indexOf<cs2::C_Flashbang>(): return kFlashbangColor;
        case EntityTypeInfo::indexOf<cs2::C_HEGrenade>(): return kHEGrenadeColor;
        case EntityTypeInfo::indexOf<cs2::C_SmokeGrenade>(): return kSmokeGrenadeColor;
        default: return kDefaultWeaponColor;
        }
    }

    Context context;
};
