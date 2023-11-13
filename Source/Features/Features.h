#pragma once

#include <Helpers/PlantedC4Provider.h>

#include "Hud/HudFeatures.h"
#include "Sound/SoundFeatures.h"
#include "Visuals/VisualFeatures.h"

struct Features {
    Features(LoopModeGameHook& loopModeGameHook, ViewRenderHook& viewRenderHook, SoundWatcher& soundWatcher) noexcept
        : visuals{ loopModeGameHook }
        , soundFeatures{ viewRenderHook, soundWatcher }
    {
    }

    HudFeatures hudFeatures;
    VisualFeatures visuals;
    SoundFeatures soundFeatures;
};
