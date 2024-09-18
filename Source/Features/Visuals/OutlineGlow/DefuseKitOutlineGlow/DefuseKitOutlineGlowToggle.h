#pragma once

#include <FeatureHelpers/FeatureToggle.h>
#include "DefuseKitOutlineGlowContext.h"

template <typename Context>
class DefuseKitOutlineGlowToggle : public FeatureToggle<DefuseKitOutlineGlowToggle<Context>> {
public:
    template <typename... Args>
     DefuseKitOutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename DefuseKitOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

private:
    Context context;
};

template <typename HookContext>
DefuseKitOutlineGlowToggle(HookContext&) -> DefuseKitOutlineGlowToggle<DefuseKitOutlineGlowContext<HookContext>>;
