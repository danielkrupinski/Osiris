#pragma once

#include <Helpers/PlantedC4Provider.h>

#include "Hud/HudFeatures.h"
#include "Sound/SoundFeatures.h"
#include "Visuals/VisualFeatures.h"

struct Features {
    Features(HudProvider hudProvider, GlobalVarsProvider globalVarsProvider, LoopModeGameHook& loopModeGameHook, ViewRenderHook& viewRenderHook) noexcept
        : hudFeatures{ PlantedC4Provider{}, hudProvider, globalVarsProvider }
        , visuals{ hudProvider, loopModeGameHook }
        , soundFeatures{ hudProvider, globalVarsProvider, viewRenderHook }
    {
    }

    HudFeatures hudFeatures;
    VisualFeatures visuals;
    SoundFeatures soundFeatures;
};
