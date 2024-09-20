#pragma once

#include <utility>

#include <CS2/Constants/ColorConstants.h>

#include "GrenadeProjectileOutlineGlowContext.h"
#include "GrenadeProjectileOutlineGlowParams.h"

template <typename Context>
class GrenadeProjectileOutlineGlow {
public:
    template <typename... Args>
    GrenadeProjectileOutlineGlow(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    void applyGlowToGrenadeProjectile(EntityTypeInfo entityTypeInfo, auto&& grenadeProjectile) const noexcept
    {
        if (context.state().enabled) {
            using namespace grenade_projectile_outline_glow_params;
            grenadeProjectile.applyGlowRecursively(getColor(entityTypeInfo).setAlpha(kColorAlpha));
        }
    }

private:
    [[nodiscard]] cs2::Color getColor(EntityTypeInfo entityTypeInfo) const noexcept
    {
        using namespace grenade_projectile_outline_glow_params;

        switch (entityTypeInfo.typeIndex) {
        case EntityTypeInfo::indexOf<cs2::C_FlashbangProjectile>(): return kFlashbangColor;
        case EntityTypeInfo::indexOf<cs2::C_HEGrenadeProjectile>(): return kHEGrenadeColor;
        case EntityTypeInfo::indexOf<cs2::C_MolotovProjectile>(): return kMolotovColor;
        case EntityTypeInfo::indexOf<cs2::C_SmokeGrenadeProjectile>(): return kSmokeGrenadeColor;
        default: return kFallbackColor;
        }
    }

    Context context;
};

template <typename HookContext>
GrenadeProjectileOutlineGlow(HookContext&) -> GrenadeProjectileOutlineGlow<GrenadeProjectileOutlineGlowContext<HookContext>>;
