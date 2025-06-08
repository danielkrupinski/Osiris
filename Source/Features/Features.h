#pragma once

#include <Hooks/Hooks.h>

#include "Sound/SoundFeatures.h"
#include "Visuals/VisualFeatures.h"
#include "Aimbot/Aimbot.h"

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

    [[nodiscard]] auto aimbot() const noexcept
    {
        // This will likely need to be adjusted once actual dependencies (like config) are figured out
        return Aimbot{};
    }

    FeaturesStates& states;
    Hooks& hooks;
    HookContext& hookContext;
};
