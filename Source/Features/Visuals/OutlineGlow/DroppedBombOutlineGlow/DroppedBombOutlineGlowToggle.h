#pragma once

#include <utility>

#include <Features/Common/FeatureToggle.h>
#include "DroppedBombOutlineGlowContext.h"

template <typename HookContext, typename Context = DroppedBombOutlineGlowContext<HookContext>>
class DroppedBombOutlineGlowToggle : public FeatureToggle<DroppedBombOutlineGlowToggle<HookContext, Context>> {
public:
    template <typename... Args>
    DroppedBombOutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename DroppedBombOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabledForDroppedBomb;
    }

private:
    Context context;
};
