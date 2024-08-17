#pragma once

#include <FeatureHelpers/FeatureToggle.h>

template <typename Context>
struct BombTimerToggle : FeatureToggleOff<BombTimerToggle<Context>> {
    BombTimerToggle(Context context) noexcept
        : context{context}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename BombTimerToggle::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

    void onDisable(typename BombTimerToggle::ToggleMethod) const noexcept
    {
        context.bombTimerPanel().hide();
    }

private:
    Context context;
};
