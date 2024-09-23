#pragma once

#include <utility>

#include <FeatureHelpers/FeatureToggle.h>
#include "DroppedBombOutlineGlowContext.h"

template <typename Context>
class DroppedBombOutlineGlowToggle : public FeatureToggle<DroppedBombOutlineGlowToggle<Context>> {
public:
    template <typename... Args>
    DroppedBombOutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename DroppedBombOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

private:
    Context context;
};

template <typename HookContext>
DroppedBombOutlineGlowToggle(HookContext&) -> DroppedBombOutlineGlowToggle<DroppedBombOutlineGlowContext<HookContext>>;
