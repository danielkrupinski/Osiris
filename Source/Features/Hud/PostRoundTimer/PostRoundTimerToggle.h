#pragma once

#include <utility>

#include <FeatureHelpers/FeatureToggle.h>
#include "PostRoundTimerContext.h"

template <typename Context>
struct PostRoundTimerToggle : FeatureToggleOff<PostRoundTimerToggle<Context>> {
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

template <typename HookContext>
PostRoundTimerToggle(HookContext&) -> PostRoundTimerToggle<PostRoundTimerContext<HookContext>>;
