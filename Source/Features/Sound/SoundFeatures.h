#pragma once

#include "BombPlantVisualizer.h"
#include "FootstepVisualizer.h"
#include <Helpers/HudProvider.h>
#include <Hooks/ViewRenderHook.h>

struct SoundFeatures {
    SoundFeatures(HudProvider hudProvider, GlobalVarsProvider globalVarsProvider, ViewRenderHook& viewRenderHook, SoundWatcher& soundWatcher) noexcept
        : footstepVisualizer{ hudProvider, globalVarsProvider, viewRenderHook, soundWatcher }
        , bombPlantVisualizer{ hudProvider, globalVarsProvider, viewRenderHook, soundWatcher }
    {
    }
    
    FootstepVisualizer footstepVisualizer;
    BombPlantVisualizer bombPlantVisualizer;
};
