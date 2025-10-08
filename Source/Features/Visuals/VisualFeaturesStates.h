#pragma once

#include "ModelGlow/ModelGlowState.h"
#include "PlayerInfoInWorld/PlayerInfoInWorldState.h"
#include "ViewmodelMod/ViewmodelModState.h"

struct VisualFeaturesStates {
    PlayerInfoInWorldState playerInfoInWorldState;
    ModelGlowState modelGlowState;
    ViewmodelModState viewmodelModState;
};
