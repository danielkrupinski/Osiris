#pragma once

#include <CS2/Constants/ColorConstants.h>
#include "WeaponOutlineGlowContext.h"

template <typename Context>
class WeaponOutlineGlow {
public:
    template <typename... Args>
    WeaponOutlineGlow(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }
    
    void applyGlowToWeapon(auto&& weapon) const noexcept
    {
        auto&& condition = context.condition();
        if (!condition.shouldRun() || !condition.shouldGlowWeapon(weapon))
            return;

        constexpr auto color = cs2::kColorWhite.setAlpha(102);
        constexpr auto range = 800;
        weapon.applyGlow(color, range);
        weapon.forEachChild([color, range](auto&& entity) { entity.applyGlow(color, range); });
    }

private:
    Context context;
};

template <typename HookContext>
WeaponOutlineGlow(HookContext&) -> WeaponOutlineGlow<WeaponOutlineGlowContext<HookContext>>;
