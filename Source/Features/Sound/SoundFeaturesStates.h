#pragma once

#include "BombBeepVisualizer.h"
#include "BombDefuseVisualizer.h"
#include "BombPlantVisualizer.h"
#include "FootstepVisualizer.h"
#include "WeaponReloadVisualizer.h"
#include "WeaponScopeVisualizer.h"

struct SoundFeaturesStates {
    SoundVisualizationFeatureState footstepVisualizerState;
    SoundVisualizationFeatureState bombPlantVisualizerState;
    SoundVisualizationFeatureState bombBeepVisualizerState;
    SoundVisualizationFeatureState bombDefuseVisualizerState;
    SoundVisualizationFeatureState weaponScopeVisualizerState;
    SoundVisualizationFeatureState weaponReloadVisualizerState;
};
