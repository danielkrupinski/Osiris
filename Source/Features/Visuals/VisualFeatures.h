#pragma once

#include <FeatureHelpers/FeatureHelpers.h>
#include "PlayerPositionThroughWalls.h"
#include "States/VisualFeaturesStates.h"

class LoopModeGameHook;

struct VisualFeatures {
    [[nodiscard]] PlayerPositionThroughWallsToggle playerPositionThroughWalls() const noexcept
    {
        return PlayerPositionThroughWallsToggle{states.playerPositionThroughWallsState, helpers.hudInWorldPanelContainer, viewRenderHook, helpers.panelConfigurator(), helpers.hudProvider};
    }

    VisualFeaturesStates& states;
    FeatureHelpers& helpers;
    ViewRenderHook& viewRenderHook;
};
