#pragma once

#include "FootstepVisualizer.h"
#include <Helpers/HudProvider.h>
#include <Hooks/ViewRenderHook.h>

struct SoundFeatures {
    SoundFeatures(HudProvider hudProvider, GlobalVarsProvider globalVarsProvider, ViewRenderHook& viewRenderHook) noexcept
        : footstepVisualizer{ hudProvider, globalVarsProvider, viewRenderHook }
    {
    }
    
    FootstepVisualizer footstepVisualizer;
};
