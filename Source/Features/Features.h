#pragma once

#include <Hooks/Hooks.h>

#include "Sound/SoundFeatures.h"

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

    FeaturesStates& states;
    Hooks& hooks;
    HookContext& hookContext;
};
