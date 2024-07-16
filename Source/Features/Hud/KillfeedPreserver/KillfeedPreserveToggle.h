#pragma once

#include <FeatureHelpers/FeatureToggle.h>

template <typename Context>
struct KillfeedPreserveToggle : FeatureToggle<KillfeedPreserveToggle<Context>> {
    explicit KillfeedPreserveToggle(Context context) noexcept
        : context{context}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename KillfeedPreserveToggle::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

private:
    Context context;
};
