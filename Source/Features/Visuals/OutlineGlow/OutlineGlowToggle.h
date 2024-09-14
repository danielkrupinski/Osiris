#pragma once

#include <FeatureHelpers/FeatureToggle.h>
#include "OutlineGlowContext.h"

template <typename Context>
class OutlineGlowToggle : public FeatureToggleOnOff<OutlineGlowToggle<Context>> {
public:
    template <typename... Args>
    OutlineGlowToggle(Args&&... args) noexcept
        : context{std::forward<Args>(args)...}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename OutlineGlowToggle::ToggleMethod) const noexcept
    {
        return context.state().enabled;
    }

    void onEnable(typename OutlineGlowToggle::ToggleMethod) noexcept
    {
        context.viewRenderHook().incrementReferenceCount();
    }

    void onDisable(typename OutlineGlowToggle::ToggleMethod) noexcept
    {
        context.viewRenderHook().decrementReferenceCount();
        context.clearGlowSceneObjects();
    }

private:
    Context context;
};

template <typename HookContext>
OutlineGlowToggle(HookContext&) -> OutlineGlowToggle<OutlineGlowContext<HookContext>>;
