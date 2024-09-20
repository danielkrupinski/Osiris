#pragma once

#include <Hooks/Hooks.h>

#include "Hud/HudFeatures.h"
#include "Sound/SoundFeatures.h"
#include "Visuals/VisualFeatures.h"

#include "FeaturesStates.h"

template <typename HookContext>
struct Features {
    Features(FeaturesStates& states, FeatureHelpers& helpers, Hooks& hooks, HookContext& hookContext) noexcept
        : states{states}
        , helpers{helpers}
        , hooks{hooks}
        , hookContext{hookContext}
    {
    }

    [[nodiscard]] auto hudFeatures() const noexcept
    {
        return HudFeatures{states.hudFeaturesStates, hookContext};
    }

    [[nodiscard]] auto soundFeatures() const noexcept
    {
        return SoundFeatures{states.soundFeaturesStates, helpers, hooks.viewRenderHook, hookContext};
    }

    [[nodiscard]] auto visualFeatures() const noexcept
    {
        return VisualFeatures{hookContext, states.visualFeaturesStates, hooks.viewRenderHook};
    }

    FeaturesStates& states;
    FeatureHelpers& helpers;
    Hooks& hooks;
    HookContext& hookContext;
};
