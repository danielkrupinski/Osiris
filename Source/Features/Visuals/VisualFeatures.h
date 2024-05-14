#pragma once

#include <FeatureHelpers/FeatureHelpers.h>
#include "PlayerInformationThroughWalls/PlayerInformationThroughWalls.h"
#include "VisualFeaturesStates.h"

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

    [[nodiscard]] PlayerPositionArrowColorToggle playerPositionArrowColorToggle() const noexcept
    {
        return PlayerPositionArrowColorToggle{states.playerInformationThroughWallsState.playerPositionArrowColor};
    }

    [[nodiscard]] PlayerHealthToggle playerHealthToggle() const noexcept
    {
        return PlayerHealthToggle{states.playerInformationThroughWallsState};
    }

    [[nodiscard]] PlayerHealthTextColorToggle playerHealthTextColorToggle() const noexcept
    {
        return PlayerHealthTextColorToggle{states.playerInformationThroughWallsState.playerHealthTextColor};
    }

    [[nodiscard]] PlayerActiveWeaponToggle playerActiveWeaponToggle() const noexcept
    {
        return PlayerActiveWeaponToggle{states.playerInformationThroughWallsState};
    }

    [[nodiscard]] PlayerDefuseIconToggle playerDefuseIconToggle() const noexcept
    {
        return PlayerDefuseIconToggle{states.playerInformationThroughWallsState.playerStateIconsToShow};
    }

    [[nodiscard]] HostagePickupIconToggle hostagePickupIconToggle() const noexcept
    {
        return HostagePickupIconToggle{states.playerInformationThroughWallsState.playerStateIconsToShow};
    }

    [[nodiscard]] HostageRescueIconToggle hostageRescueIconToggle() const noexcept
    {
        return HostageRescueIconToggle{states.playerInformationThroughWallsState.playerStateIconsToShow};
    }

    VisualFeaturesStates& states;
    FeatureHelpers& helpers;
    ViewRenderHook& viewRenderHook;
};
