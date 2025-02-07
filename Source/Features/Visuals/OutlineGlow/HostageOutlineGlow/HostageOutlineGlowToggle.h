#pragma once

#include <utility>
#include <Features/Common/FeatureToggle.h>
#include "HostageOutlineGlowContext.h"

template <typename HookContext, typename Context = HostageOutlineGlowContext<HookContext>>
class HostageOutlineGlowToggle : public FeatureToggle<HostageOutlineGlowToggle<HookContext, Context>> {
public:
    template <typename... Args>
    HostageOutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename HostageOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabledForHostages;
    }

private:
    Context context;
};
