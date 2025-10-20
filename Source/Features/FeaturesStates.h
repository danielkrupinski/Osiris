#pragma once

#include "Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVisState.h"
#include "Hud/HudFeaturesStates.h"
#include "Visuals/VisualFeaturesStates.h"

struct FeaturesStates {
    HudFeaturesStates hudFeaturesStates;
    VisualFeaturesStates visualFeaturesStates;
    NoScopeInaccuracyVisState noScopeInaccuracyVisState;
};
