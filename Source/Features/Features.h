#pragma once

#include <Hooks/Hooks.h>

#include "Hud/HudFeatures.h"
#include "Sound/SoundFeatures.h"
#include "Visuals/VisualFeatures.h"

#include "FeaturesStates.h"

struct Features {
    [[nodiscard]] HudFeatures hudFeatures() const noexcept
    {
        return HudFeatures{states.hudFeaturesStates, helpers, hookDependencies};
    }

    [[nodiscard]] SoundFeatures soundFeatures() const noexcept
    {
        return SoundFeatures{states.soundFeaturesStates, helpers, hooks.viewRenderHook, hookDependencies};
    }

    [[nodiscard]] VisualFeatures visualFeatures() const noexcept
    {
        return VisualFeatures{states.visualFeaturesStates, helpers, hooks.viewRenderHook};
    }

    FeaturesStates& states;
    FeatureHelpers& helpers;
    Hooks& hooks;
    HookDependencies& hookDependencies;
};
