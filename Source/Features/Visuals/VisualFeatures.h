#pragma once

#include <FeatureHelpers/FeatureHelpers.h>
#include "ScopeOverlayRemover/ScopeOverlayRemover.h"
#include "SniperScopeBlurRemovalFeature.h"
#include "VisualFeaturesStates.h"

class LoopModeGameHook;

struct VisualFeatures {
    [[nodiscard]] ScopeOverlayRemover scopeOverlayRemover() const noexcept
    {
        return ScopeOverlayRemover{states.scopeOverlayRemoverState, helpers.mainMenu, helpers.hudScope, loopModeGameHook, helpers.sniperScopeBlurRemover};
    }

    [[nodiscard]] SniperScopeBlurRemovalFeature sniperScopeBlurRemoval() const noexcept
    {
        return SniperScopeBlurRemovalFeature{states.sniperScopeBlurRemovalFeatureState, loopModeGameHook, helpers.sniperScopeBlurRemover};
    }

    VisualFeaturesStates& states;
    FeatureHelpers& helpers;
    LoopModeGameHook& loopModeGameHook;
};
