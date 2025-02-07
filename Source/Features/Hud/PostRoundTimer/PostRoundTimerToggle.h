#pragma once

#include <utility>

#include <Features/Common/FeatureToggle.h>
#include "PostRoundTimerContext.h"

template <typename HookContext, typename Context = PostRoundTimerContext<HookContext>>
struct PostRoundTimerToggle : FeatureToggleOff<PostRoundTimerToggle<HookContext, Context>> {
    template <typename... Args>
    PostRoundTimerToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename PostRoundTimerToggle::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

    void onDisable(typename PostRoundTimerToggle::ToggleMethod) const noexcept
    {
        context.postRoundTimerPanel().hide();
    }

private:
    Context context;
};
