#pragma once

#include <Features/Sound/Details/SoundWatcher.h>
#include <Features/Sound/Details/SoundWatcherState.h>
#include <Hooks/ViewRenderHook.h>

#include "Details/BombBeepSound.h"
#include "Details/BombDefuseSound.h"
#include "Details/BombPlantSound.h"
#include "Details/FootstepSound.h"
#include "Details/WeaponReloadSound.h"
#include "Details/WeaponScopeSound.h"
#include "BombBeepVisualizer.h"
#include "BombDefuseVisualizer.h"
#include "BombPlantVisualizer.h"
#include "FootstepVisualizer.h"
#include "WeaponReloadVisualizer.h"
#include "WeaponScopeVisualizer.h"

template <typename HookContext>
struct SoundFeatures {
    SoundFeatures(SoundWatcherState& soundWatcherState, ViewRenderHook& viewRenderHook, HookContext& hookContext) noexcept
        : soundWatcherState{soundWatcherState}
        , viewRenderHook{viewRenderHook}
        , hookContext{hookContext}
    {
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

private:
    [[nodiscard]] auto footstepVisualizer() const noexcept
    {
        return soundVisualizationFeature<FootstepVisualizer>();
    }

    [[nodiscard]] auto bombPlantVisualizer() const noexcept
    {
        return soundVisualizationFeature<BombPlantVisualizer>();
    }

    [[nodiscard]] auto bombBeepVisualizer() const noexcept
    {
        return soundVisualizationFeature<BombBeepVisualizer>();
    }

    [[nodiscard]] auto bombDefuseVisualizer() const noexcept
    {
        return soundVisualizationFeature<BombDefuseVisualizer>();
    }

    [[nodiscard]] auto weaponScopeVisualizer() const noexcept
    {
        return soundVisualizationFeature<WeaponScopeVisualizer>();
    }

    [[nodiscard]] auto weaponReloadVisualizer() const noexcept
    {
        return soundVisualizationFeature<WeaponReloadVisualizer>();
    }

    template <template <typename> typename SoundVisualizationFeature>
    [[nodiscard]] auto soundVisualizationFeature() const noexcept
    {
        return SoundVisualizationFeature<HookContext>{
            hookContext,
            viewRenderHook,
            SoundWatcher<HookContext>{soundWatcherState, hookContext},
        };
    }

    SoundWatcherState& soundWatcherState;
    ViewRenderHook& viewRenderHook;
    HookContext& hookContext;
};
