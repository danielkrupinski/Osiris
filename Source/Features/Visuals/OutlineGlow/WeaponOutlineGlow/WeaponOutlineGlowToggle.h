#pragma once

#include <FeatureHelpers/FeatureToggle.h>
#include "WeaponOutlineGlowContext.h"

template <typename Context>
class WeaponOutlineGlowToggle : public FeatureToggle<WeaponOutlineGlowToggle<Context>> {
public:
    template <typename... Args>
    WeaponOutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename WeaponOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

private:
    Context context;
};

template <typename HookContext>
WeaponOutlineGlowToggle(HookContext&) -> WeaponOutlineGlowToggle<WeaponOutlineGlowContext<HookContext>>;
