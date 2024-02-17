#pragma once

#include <Helpers/PlantedC4Provider.h>
#include <Hooks/Hooks.h>

#include "Hud/HudFeatures.h"
#include "Sound/SoundFeatures.h"
#include "Visuals/VisualFeatures.h"

#include "FeaturesStates.h"

struct Features {
    [[nodiscard]] HudFeatures hudFeatures() const noexcept
    {
        return HudFeatures{states.hudFeaturesStates, helpers};
    }

    [[nodiscard]] SoundFeatures soundFeatures() const noexcept
    {
        return SoundFeatures{states.soundFeaturesStates, helpers, hooks.viewRenderHook};
    }

    [[nodiscard]] VisualFeatures visualFeatures() const noexcept
    {
        return VisualFeatures{states.visualFeaturesStates, helpers};
    }

    FeaturesStates& states;
    FeatureHelpers& helpers;
    Hooks& hooks;
};
