#pragma once

#include <Hooks/Hooks.h>

#include "Sound/SoundFeatures.h"
#include "Visuals/VisualFeatures.h"

#include "FeaturesStates.h"

template <typename HookContext>
struct Features {
    Features(FeaturesStates& states, Hooks& hooks, HookContext& hookContext) noexcept
        : states{states}
        , hooks{hooks}
        , hookContext{hookContext}
    {
    }

    [[nodiscard]] auto soundFeatures() const noexcept
    {
        return SoundFeatures{hookContext.soundWatcherState(), hooks.viewRenderHook, hookContext};
    }

    [[nodiscard]] auto visualFeatures() const noexcept
    {
        return VisualFeatures{hookContext, states.visualFeaturesStates, hooks.viewRenderHook};
    }

    FeaturesStates& states;
    Hooks& hooks;
    HookContext& hookContext;
};
