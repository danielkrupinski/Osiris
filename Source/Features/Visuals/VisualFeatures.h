#pragma once

#include <FeatureHelpers/FeatureHelpers.h>
#include "ScopeOverlayRemover.h"
#include "States/VisualFeaturesStates.h"

class LoopModeGameHook;

struct VisualFeatures {
    [[nodiscard]] ScopeOverlayRemover scopeOverlayRemover() const noexcept
    {
        return ScopeOverlayRemover{states.scopeOverlayRemoverState, helpers.mainMenuProvider, helpers.hudProvider, helpers.hudScope};
    }

    VisualFeaturesStates& states;
    FeatureHelpers& helpers;
};
