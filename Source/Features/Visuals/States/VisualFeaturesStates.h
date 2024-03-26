#pragma once

#include "PlayerPositionThroughWallsState.h"
#include "ScopeOverlayRemoverState.h"

struct VisualFeaturesStates {
    ScopeOverlayRemoverState scopeOverlayRemoverState;
    PlayerPositionThroughWallsState playerPositionThroughWallsState;
};
