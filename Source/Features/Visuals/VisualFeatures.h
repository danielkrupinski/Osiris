#pragma once

#include <FeatureHelpers/FeatureHelpers.h>
#include "ScopeOverlayRemover.h"
#include "PlayerPositionThroughWalls.h"
#include "States/VisualFeaturesStates.h"

class LoopModeGameHook;

struct VisualFeatures {
    [[nodiscard]] ScopeOverlayRemover scopeOverlayRemover() const noexcept
    {
        return ScopeOverlayRemover{states.scopeOverlayRemoverState, helpers.mainMenuProvider, helpers.hudProvider, helpers.hudScope};
    }

    [[nodiscard]] PlayerPositionThroughWallsToggle playerPositionThroughWalls() const noexcept
    {
        return PlayerPositionThroughWallsToggle{states.playerPositionThroughWallsState, helpers.hudInWorldPanelContainer, viewRenderHook, helpers.panelConfigurator(), helpers.hudProvider};
    }

    VisualFeaturesStates& states;
    FeatureHelpers& helpers;
    ViewRenderHook& viewRenderHook;
};
