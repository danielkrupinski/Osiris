#pragma once

#include "BombBeepVisualizer.h"
#include "BombDefuseVisualizer.h"
#include "BombPlantVisualizer.h"
#include "FootstepVisualizer.h"
#include "SoundFeaturesStates.h"
#include "WeaponReloadVisualizer.h"
#include "WeaponScopeVisualizer.h"
#include <Hooks/ViewRenderHook.h>

struct SoundFeatures {
    [[nodiscard]] FootstepVisualizer footstepVisualizer() const noexcept
    {
        return soundVisualizationFeature<FootstepVisualizer>(states.footstepVisualizerState);
    }

    [[nodiscard]] BombPlantVisualizer bombPlantVisualizer() const noexcept
    {
        return soundVisualizationFeature<BombPlantVisualizer>(states.bombPlantVisualizerState);
    }

    [[nodiscard]] BombBeepVisualizer bombBeepVisualizer() const noexcept
    {
        return soundVisualizationFeature<BombBeepVisualizer>(states.bombBeepVisualizerState);
    }

    [[nodiscard]] BombDefuseVisualizer bombDefuseVisualizer() const noexcept
    {
        return soundVisualizationFeature<BombDefuseVisualizer>(states.bombDefuseVisualizerState);
    }

    [[nodiscard]] WeaponScopeVisualizer weaponScopeVisualizer() const noexcept
    {
        return soundVisualizationFeature<WeaponScopeVisualizer>(states.weaponScopeVisualizerState);
    }

    [[nodiscard]] WeaponReloadVisualizer weaponReloadVisualizer() const noexcept
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
    HookDependencies& hookDependencies;

private:
    template <typename SoundVisualizationFeature>
    [[nodiscard]] SoundVisualizationFeature soundVisualizationFeature(auto& state) const noexcept
    {
        return SoundVisualizationFeature{
            state,
            hookDependencies,
            viewRenderHook,
            SoundWatcher{helpers.soundWatcherState, hookDependencies},
            helpers.hudInWorldPanelContainer
        };
    }

    template <typename SoundType>
    [[nodiscard]] SoundVisualizationFeatureToggle<SoundType> soundVisualizationFeatureToggle(auto& state) const noexcept
    {
        return SoundVisualizationFeatureToggle<SoundType>{
            state,
            hookDependencies,
            SoundWatcher{helpers.soundWatcherState, hookDependencies},
            viewRenderHook,
            helpers.hudInWorldPanelContainer
        };
    }
};
