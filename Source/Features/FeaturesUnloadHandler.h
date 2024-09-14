#pragma once

#include "FeaturesStates.h"
#include "Hud/HudFeaturesUnloadHandler.h"
#include "Visuals/OutlineGlow/OutlineGlowUnloadHandler.h"

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
        hookContext.template make<OutlineGlowUnloadHandler>().handleUnload();
    }

    HookContext& hookContext;
    FeaturesStates& states;
};
