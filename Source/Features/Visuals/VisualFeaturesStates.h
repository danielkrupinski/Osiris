#pragma once

#include "ModelGlow/ModelGlowState.h"
#include "PlayerInfoInWorld/PlayerInfoInWorldState.h"
#include "OutlineGlow/OutlineGlowState.h"

struct VisualFeaturesStates {
    PlayerInfoInWorldState playerInfoInWorldState;
    OutlineGlowState outlineGlowState;
    ModelGlowState modelGlowState;
};
