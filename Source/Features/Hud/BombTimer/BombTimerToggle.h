#pragma once

#include <FeatureHelpers/FeatureToggle.h>

template <typename Context>
struct BombTimerToggle : FeatureToggleOff<BombTimerToggle<Context>> {
    BombTimerToggle(Context context) noexcept
        : context{context}
    {
    }

    [[nodiscard]] auto& enabledVariable(FeatureToggleOff<BombTimerToggle<Context>>::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

    void onDisable(FeatureToggleOff<BombTimerToggle<Context>>::ToggleMethod) const noexcept
    {
        context.gameBombStatusPanel().restore();
        context.bombTimerPanel().hide();
    }

private:
    Context context;
};
