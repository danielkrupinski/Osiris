#pragma once

#include <utility>
#include <Features/Common/FeatureToggle.h>
#include "DefuseKitOutlineGlowContext.h"

template <typename HookContext, typename Context = DefuseKitOutlineGlowContext<HookContext>>
class DefuseKitOutlineGlowToggle : public FeatureToggle<DefuseKitOutlineGlowToggle<HookContext, Context>> {
public:
    template <typename... Args>
     DefuseKitOutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename DefuseKitOutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabledForDefuseKits;
    }

private:
    Context context;
};
