#pragma once

#include "BombBeepVisualizer.h"
#include "BombDefuseVisualizer.h"
#include "BombPlantVisualizer.h"
#include "FootstepVisualizer.h"
#include "WeaponReloadVisualizer.h"
#include "WeaponScopeVisualizer.h"

struct SoundFeaturesStates {
    FootstepVisualizerState footstepVisualizerState;
    BombPlantVisualizerState bombPlantVisualizerState;
    BombBeepVisualizerState bombBeepVisualizerState;
    BombDefuseVisualizerState bombDefuseVisualizerState;
    WeaponScopeVisualizerState weaponScopeVisualizerState;
    WeaponReloadVisualizerState weaponReloadVisualizerState;
};
