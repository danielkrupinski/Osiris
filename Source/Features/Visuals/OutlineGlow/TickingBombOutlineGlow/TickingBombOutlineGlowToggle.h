#pragma once

#include <utility>

#include <FeatureHelpers/FeatureToggle.h>
#include "TickingBombOutlineGlowContext.h"

template <typename Context>
class TickingBombOutlineGlowToggle : public FeatureToggle<TickingBombOutlineGlowToggle<Context>> {
public:
    template <typename... Args>
    TickingBombOutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename TickingBombOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

private:
    Context context;
};

template <typename HookContext>
TickingBombOutlineGlowToggle(HookContext&) -> TickingBombOutlineGlowToggle<TickingBombOutlineGlowContext<HookContext>>;
