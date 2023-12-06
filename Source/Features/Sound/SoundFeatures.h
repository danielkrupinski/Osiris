#pragma once

#include "BombBeepVisualizer.h"
#include "BombDefuseVisualizer.h"
#include "BombPlantVisualizer.h"
#include "FootstepVisualizer.h"
#include "WeaponReloadVisualizer.h"
#include "WeaponScopeVisualizer.h"
#include <Hooks/ViewRenderHook.h>

struct SoundFeatures {
    SoundFeatures(ViewRenderHook& viewRenderHook, SoundWatcher& soundWatcher) noexcept
        : footstepVisualizer{ viewRenderHook, soundWatcher }
        , bombPlantVisualizer{ viewRenderHook, soundWatcher }
        , bombBeepVisualizer{ viewRenderHook, soundWatcher }
        , bombDefuseVisualizer{ viewRenderHook, soundWatcher }
        , weaponScopeVisualizer{ viewRenderHook, soundWatcher }
        , weaponReloadVisualizer{ viewRenderHook, soundWatcher }
    {
    }
    
    void runOnViewMatrixUpdate(const SoundVisualizationHelpers& soundVisualizationHelpers) noexcept
    {
        footstepVisualizer.run(soundVisualizationHelpers);
        bombPlantVisualizer.run(soundVisualizationHelpers);
        bombBeepVisualizer.run(soundVisualizationHelpers);
        bombDefuseVisualizer.run(soundVisualizationHelpers);
        weaponScopeVisualizer.run(soundVisualizationHelpers);
        weaponReloadVisualizer.run(soundVisualizationHelpers);
    }

    FootstepVisualizer footstepVisualizer;
    BombPlantVisualizer bombPlantVisualizer;
    BombBeepVisualizer bombBeepVisualizer;
    BombDefuseVisualizer bombDefuseVisualizer;
    WeaponScopeVisualizer weaponScopeVisualizer;
    WeaponReloadVisualizer weaponReloadVisualizer;
};
