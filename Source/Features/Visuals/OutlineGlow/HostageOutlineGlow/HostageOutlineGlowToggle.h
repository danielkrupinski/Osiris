#pragma once

#include <FeatureHelpers/FeatureToggle.h>
#include "HostageOutlineGlowContext.h"

template <typename Context>
class HostageOutlineGlowToggle : public FeatureToggle<HostageOutlineGlowToggle<Context>> {
public:
    template <typename... Args>
    HostageOutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename HostageOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

private:
    Context context;
};

template <typename HookContext>
HostageOutlineGlowToggle(HookContext&) -> HostageOutlineGlowToggle<GrenadeProjectileOutlineGlowContext<HookContext>>;
