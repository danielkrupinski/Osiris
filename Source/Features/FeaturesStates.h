#pragma once

#include "Hud/States/HudFeaturesStates.h"
#include "Sound/SoundFeaturesStates.h"
#include "Visuals/States/VisualFeaturesStates.h"

struct FeaturesStates {
    HudFeaturesStates hudFeaturesStates;
    SoundFeaturesStates soundFeaturesStates;
    VisualFeaturesStates visualFeaturesStates;
};
