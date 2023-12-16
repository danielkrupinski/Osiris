#pragma once

#include <Helpers/PlantedC4Provider.h>

#include "Hud/HudFeatures.h"
#include "Sound/SoundFeatures.h"
#include "Visuals/VisualFeatures.h"

class SniperScopeBlurRemover;

struct Features {
    Features(SniperScopeBlurRemover& sniperScopeBlurRemover, LoopModeGameHook& loopModeGameHook, ViewRenderHook& viewRenderHook, SoundWatcher& soundWatcher) noexcept
        : visuals{sniperScopeBlurRemover, loopModeGameHook }
        , soundFeatures{ viewRenderHook, soundWatcher }
    {
    }

    HudFeatures hudFeatures;
    VisualFeatures visuals;
    SoundFeatures soundFeatures;
};
