#pragma once

#include "BombBeepVisualizer.h"
#include "BombDefuseVisualizer.h"
#include "BombPlantVisualizer.h"
#include "FootstepVisualizer.h"
#include "SoundFeaturesStates.h"
#include "WeaponReloadVisualizer.h"
#include "WeaponScopeVisualizer.h"
#include <Hooks/ViewRenderHook.h>

template <typename HookContext>
struct SoundFeatures {
    SoundFeatures(SoundFeaturesStates& states, FeatureHelpers& helpers, ViewRenderHook& viewRenderHook, HookContext& hookContext) noexcept
        : states{states}
        , helpers{helpers}
        , viewRenderHook{viewRenderHook}
        , hookContext{hookContext}
    {
    }

    [[nodiscard]] auto footstepVisualizer() const noexcept
    {
        return soundVisualizationFeature<FootstepVisualizer>(states.footstepVisualizerState);
    }

    [[nodiscard]] auto bombPlantVisualizer() const noexcept
    {
        return soundVisualizationFeature<BombPlantVisualizer>(states.bombPlantVisualizerState);
    }

    [[nodiscard]] auto bombBeepVisualizer() const noexcept
    {
        return soundVisualizationFeature<BombBeepVisualizer>(states.bombBeepVisualizerState);
    }

    [[nodiscard]] auto bombDefuseVisualizer() const noexcept
    {
        return soundVisualizationFeature<BombDefuseVisualizer>(states.bombDefuseVisualizerState);
    }

    [[nodiscard]] auto weaponScopeVisualizer() const noexcept
    {
        return soundVisualizationFeature<WeaponScopeVisualizer>(states.weaponScopeVisualizerState);
    }

    [[nodiscard]] auto weaponReloadVisualizer() const noexcept
    {
        return soundVisualizationFeature<WeaponReloadVisualizer>(states.weaponReloadVisualizerState);
    }

    [[nodiscard]] auto footstepVisualizerToggle() const noexcept
    {
        return soundVisualizationFeatureToggle<FootstepSound>(states.footstepVisualizerState);
    }

    [[nodiscard]] auto bombPlantVisualizerToggle() const noexcept
    {
        return soundVisualizationFeatureToggle<BombPlantSound>(states.bombPlantVisualizerState);
    }

    [[nodiscard]] auto bombBeepVisualizerToggle() const noexcept
    {
        return soundVisualizationFeatureToggle<BombBeepSound>(states.bombBeepVisualizerState);
    }

    [[nodiscard]] auto bombDefuseVisualizerToggle() const noexcept
    {
        return soundVisualizationFeatureToggle<BombDefuseSound>(states.bombDefuseVisualizerState);
    }

    [[nodiscard]] auto weaponScopeVisualizerToggle() const noexcept
    {
        return soundVisualizationFeatureToggle<WeaponScopeSound>(states.weaponScopeVisualizerState);
    }

    [[nodiscard]] auto weaponReloadVisualizerToggle() const noexcept
    {
        return soundVisualizationFeatureToggle<WeaponReloadSound>(states.weaponReloadVisualizerState);
    }

    void runOnViewMatrixUpdate() noexcept
    {
        footstepVisualizer().run();
        bombPlantVisualizer().run();
        bombBeepVisualizer().run();
        bombDefuseVisualizer().run();
        weaponScopeVisualizer().run();
        weaponReloadVisualizer().run();
    }

    SoundFeaturesStates& states;
    FeatureHelpers& helpers;
    ViewRenderHook& viewRenderHook;
    HookContext& hookContext;

private:
    template <template <typename> typename SoundVisualizationFeature>
    [[nodiscard]] auto soundVisualizationFeature(auto& state) const noexcept
    {
        return SoundVisualizationFeature<HookContext>{
            state,
            hookContext,
            viewRenderHook,
            SoundWatcher<HookContext>{helpers.soundWatcherState, hookContext},
        };
    }

    template <typename SoundType>
    [[nodiscard]] SoundVisualizationFeatureToggle<HookContext, SoundType> soundVisualizationFeatureToggle(auto& state) const noexcept
    {
        return SoundVisualizationFeatureToggle<HookContext, SoundType>{
            state,
            hookContext,
            SoundWatcher<HookContext>{helpers.soundWatcherState, hookContext},
            viewRenderHook,
        };
    }
};
