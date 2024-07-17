#pragma once

#include <FeatureHelpers/FeatureToggle.h>

template <typename Context>
struct DefusingAlertToggle : FeatureToggleOff<DefusingAlertToggle<Context>> {
    explicit DefusingAlertToggle(Context context) noexcept
        : context{context}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename DefusingAlertToggle::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

    void onDisable(typename DefusingAlertToggle::ToggleMethod) const noexcept
    {
        context.defusingAlertPanel().hide();
    }

private:
    Context context;
};
