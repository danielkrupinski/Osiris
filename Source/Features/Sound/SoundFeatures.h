#pragma once

#include "FootstepVisualizer.h"
#include <Helpers/HudProvider.h>
#include <Hooks/ViewRenderHook.h>

struct SoundFeatures {
    explicit SoundFeatures(HudProvider hudProvider, ViewRenderHook& viewRenderHook) noexcept
        : footstepVisualizer{ hudProvider, viewRenderHook }
    {
    }
    
    FootstepVisualizer footstepVisualizer;
};
