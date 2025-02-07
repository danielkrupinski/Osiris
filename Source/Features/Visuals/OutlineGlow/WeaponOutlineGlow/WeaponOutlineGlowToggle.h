#pragma once

#include <utility>

#include <Features/Common/FeatureToggle.h>
#include "WeaponOutlineGlowContext.h"

template <typename HookContext, typename Context = WeaponOutlineGlowContext<HookContext>>
class WeaponOutlineGlowToggle : public FeatureToggle<WeaponOutlineGlowToggle<HookContext, Context>> {
public:
    template <typename... Args>
    WeaponOutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename WeaponOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabledForWeapons;
    }

private:
    Context context;
};
