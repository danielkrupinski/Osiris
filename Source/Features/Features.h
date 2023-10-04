#pragma once

#include <Helpers/PlantedC4Provider.h>

#include "Hud/HudFeatures.h"
#include "Visuals/VisualFeatures.h"

struct Features {
    Features(HudProvider hudProvider, LoopModeGameHook& loopModeGameHook) noexcept
        : hudFeatures{ PlantedC4Provider{}, hudProvider }, visuals{ hudProvider, loopModeGameHook }
    {
    }

    HudFeatures hudFeatures;
    VisualFeatures visuals;
};
