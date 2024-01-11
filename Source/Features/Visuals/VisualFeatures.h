#pragma once

#include <FeatureHelpers/FeatureHelpers.h>
#include "ScopeOverlayRemover.h"
#include "SniperScopeBlurRemovalFeature.h"
#include "States/VisualFeaturesStates.h"

class LoopModeGameHook;

struct VisualFeatures {
    [[nodiscard]] ScopeOverlayRemover scopeOverlayRemover() const noexcept
    {
        return ScopeOverlayRemover{states.scopeOverlayRemoverState, helpers.mainMenu, helpers.hudProvider, helpers.hudScope, loopModeGameHook, helpers.sniperScopeBlurRemover};
    }

    [[nodiscard]] SniperScopeBlurRemovalFeature sniperScopeBlurRemoval() const noexcept
    {
        return SniperScopeBlurRemovalFeature{states.sniperScopeBlurRemovalFeatureState, loopModeGameHook, helpers.sniperScopeBlurRemover};
    }

    VisualFeaturesStates& states;
    FeatureHelpers& helpers;
    LoopModeGameHook& loopModeGameHook;
};
