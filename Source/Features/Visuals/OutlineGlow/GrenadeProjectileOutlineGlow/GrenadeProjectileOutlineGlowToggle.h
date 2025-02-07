#pragma once

#include <utility>
#include <Features/Common/FeatureToggle.h>
#include "GrenadeProjectileOutlineGlowContext.h"

template <typename HookContext, typename Context = GrenadeProjectileOutlineGlowContext<HookContext>>
class GrenadeProjectileOutlineGlowToggle : public FeatureToggle<GrenadeProjectileOutlineGlowToggle<HookContext, Context>> {
public:
    template <typename... Args>
    GrenadeProjectileOutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename GrenadeProjectileOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabledForGrenadeProjectiles;
    }

private:
    Context context;
};
