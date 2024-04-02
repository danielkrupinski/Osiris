#pragma once

#include <FeatureHelpers/FeatureHelpers.h>
#include "PlayerInformationThroughWalls.h"
#include "States/VisualFeaturesStates.h"

class LoopModeGameHook;

struct VisualFeatures {
    [[nodiscard]] PlayerInformationThroughWallsToggle playerInformationThroughWalls() const noexcept
    {
        return PlayerInformationThroughWallsToggle{states.playerInformationThroughWallsState, helpers.hudInWorldPanelContainer, viewRenderHook, helpers.panelConfigurator(), helpers.hudProvider};
    }

    [[nodiscard]] PlayerPositionToggle playerPositionToggle() const noexcept
    {
        return PlayerPositionToggle{states.playerInformationThroughWallsState};
    }

    [[nodiscard]] PlayerHealthToggle playerHealthToggle() const noexcept
    {
        return PlayerHealthToggle{states.playerInformationThroughWallsState};
    }

    VisualFeaturesStates& states;
    FeatureHelpers& helpers;
    ViewRenderHook& viewRenderHook;
};
