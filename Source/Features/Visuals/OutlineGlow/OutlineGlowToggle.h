#pragma once

#include <utility>

#include <Features/Common/FeatureToggle.h>
#include "OutlineGlowContext.h"

template <typename HookContext, typename Context = OutlineGlowContext<HookContext>>
class OutlineGlowToggle : public FeatureToggleOff<OutlineGlowToggle<HookContext, Context>> {
public:
    template <typename... Args>
    OutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename OutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().masterSwitch;
    }

    void onDisable(typename OutlineGlowToggle::ToggleMethod) noexcept
    {
        context.clearGlowSceneObjects();
    }

private:
    Context context;
};
