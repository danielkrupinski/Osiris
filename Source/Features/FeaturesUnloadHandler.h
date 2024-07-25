#pragma once

#include "FeaturesStates.h"
#include "Hud/HudFeaturesUnloadHandler.h"

template <typename HookContext>
struct FeaturesUnloadHandler {
    FeaturesUnloadHandler(HookContext& hookContext, FeaturesStates& states) noexcept
        : hookContext{hookContext}
        , states{states}
    {
    }

    void handleUnload() const noexcept
    {
        hookContext.template make<HudFeaturesUnloadHandler>(states.hudFeaturesStates).handleUnload();
    }

    HookContext& hookContext;
    FeaturesStates& states;
};
