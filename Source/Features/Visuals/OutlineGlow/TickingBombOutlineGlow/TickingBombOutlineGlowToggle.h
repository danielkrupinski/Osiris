#pragma once

#include <utility>

#include <Features/Common/FeatureToggle.h>
#include "TickingBombOutlineGlowContext.h"

template <typename HookContext, typename Context = TickingBombOutlineGlowContext<HookContext>>
class TickingBombOutlineGlowToggle : public FeatureToggle<TickingBombOutlineGlowToggle<HookContext, Context>> {
public:
    template <typename... Args>
    TickingBombOutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename TickingBombOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabledForTickingBomb;
    }

private:
    Context context;
};
