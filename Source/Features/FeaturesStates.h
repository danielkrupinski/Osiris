#pragma once

#include "Combat/Aimbot/AimbotState.h"
#include "Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVisState.h"
#include "Combat/Triggerbot/TriggerbotState.h"
#include "Hud/HudFeaturesStates.h"
#include "Visuals/VisualFeaturesStates.h"

struct FeaturesStates {
    HudFeaturesStates hudFeaturesStates;
    VisualFeaturesStates visualFeaturesStates;
    NoScopeInaccuracyVisState noScopeInaccuracyVisState;
    TriggerbotState triggerbotState;
    AimbotState aimbotState;
};
