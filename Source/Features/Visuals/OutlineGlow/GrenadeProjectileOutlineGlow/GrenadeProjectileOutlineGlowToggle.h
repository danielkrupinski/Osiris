#pragma once

#include <FeatureHelpers/FeatureToggle.h>
#include "GrenadeProjectileOutlineGlowContext.h"

template <typename Context>
class GrenadeProjectileOutlineGlowToggle : public FeatureToggle<GrenadeProjectileOutlineGlowToggle<Context>> {
public:
    template <typename... Args>
    GrenadeProjectileOutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename GrenadeProjectileOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

private:
    Context context;
};

template <typename HookContext>
GrenadeProjectileOutlineGlowToggle(HookContext&) -> GrenadeProjectileOutlineGlowToggle<GrenadeProjectileOutlineGlowContext<HookContext>>;
