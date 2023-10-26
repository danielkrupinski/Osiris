#pragma once

#include <Helpers/PlantedC4Provider.h>

#include "Hud/HudFeatures.h"
#include "Sound/SoundFeatures.h"
#include "Visuals/VisualFeatures.h"

struct Features {
    Features(HudProvider hudProvider, LoopModeGameHook& loopModeGameHook, ViewRenderHook& viewRenderHook) noexcept
        : hudFeatures{ PlantedC4Provider{}, hudProvider }
        , visuals{ hudProvider, loopModeGameHook }
        , soundFeatures{hudProvider, viewRenderHook }
    {
    }

    HudFeatures hudFeatures;
    VisualFeatures visuals;
    SoundFeatures soundFeatures;
};
