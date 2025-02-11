#pragma once

#include <utility>

#include <GameClient/Panorama/PanoramaTransformations.h>
#include <Features/Sound/Details/PlayedSound.h>
#include <Features/Sound/Details/SoundWatcher.h>
#include <Features/Common/FeatureToggle.h>
#include <GameClient/WorldToScreen/ViewToProjectionMatrix.h>
#include <GameClient/WorldToScreen/WorldToClipSpaceConverter.h>
#include <Features/Common/InWorldPanels.h>
#include <Hooks/ViewRenderHook.h>
#include <MemoryPatterns/PatternTypes/ClientPatternTypes.h>
#include "SoundVisualization.h"

struct SoundVisualizationFeatureState {
    bool enabled{false};
};

template <typename HookContext, typename SoundType>
struct SoundVisualizationFeatureToggle : FeatureToggleOnOff<SoundVisualizationFeatureToggle<HookContext, SoundType>> {
    SoundVisualizationFeatureToggle(SoundVisualizationFeatureState& state,
        HookContext& hookContext,
        SoundWatcher<HookContext> soundWatcher,
        ViewRenderHook& viewRenderHook)
        : state{state}
        , hookContext{hookContext}
        , soundWatcher{soundWatcher}
        , viewRenderHook{viewRenderHook}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename SoundVisualizationFeatureToggle::ToggleMethod) const noexcept
    {
        return state.enabled;
    }

    void onEnable(typename SoundVisualizationFeatureToggle::ToggleMethod) noexcept
    {
        soundWatcher.template startWatching<SoundType>();
    }

    void onDisable(typename SoundVisualizationFeatureToggle::ToggleMethod) noexcept
    {
        soundWatcher.template stopWatching<SoundType>();
    }

private:
    SoundVisualizationFeatureState& state;
    HookContext& hookContext;
    SoundWatcher<HookContext> soundWatcher;
    ViewRenderHook& viewRenderHook;
};

template <typename HookContext, typename PanelsType, typename SoundType>
class SoundVisualizationFeature {
public:
    SoundVisualizationFeature(
        SoundVisualizationFeatureState& state,
        HookContext& hookContext,
        ViewRenderHook& viewRenderHook,
        SoundWatcher<HookContext> soundWatcher) noexcept
        : state{state}
        , hookContext{hookContext}
        , viewRenderHook{viewRenderHook}
        , soundWatcher{soundWatcher}
    {
    }

    void run() noexcept
    {
        if (!state.enabled)
            return;

        const auto curtime = hookContext.globalVars().curtime();
        if (!curtime.hasValue())
            return;

        std::as_const(soundWatcher).template getSoundsOfType<SoundType>().forEach([this, curtime](const PlayedSound& sound) {
            const auto soundInClipSpace = hookContext.template make<WorldToClipSpaceConverter>().toClipSpace(sound.origin);
            if (!soundInClipSpace.onScreen())
                return;

            const auto opacity = SoundVisualization<SoundType>::getOpacity(sound.getTimeAlive(curtime.value()));
            if (opacity <= 0.0f)
                return;

            auto&& panel = hookContext.template make<InWorldPanels>().template getNextSoundVisualizationPanel<PanelsType>();
            panel.setOpacity(opacity);
            panel.setZIndex(-soundInClipSpace.z);

            const auto deviceCoordinates = soundInClipSpace.toNormalizedDeviceCoordinates();

            auto&& transformFactory = hookContext.panoramaTransformFactory();
            PanoramaTransformations{
                transformFactory.scale(SoundVisualization<SoundType>::getScale(soundInClipSpace.z, ViewToProjectionMatrix{hookContext.clientPatternSearchResults().template get<ViewToProjectionMatrixPointer>()}.getFovScale())),
                transformFactory.translate(deviceCoordinates.getX(), deviceCoordinates.getY())
            }.applyTo(panel);
        });
    }

private:
    SoundVisualizationFeatureState& state;
    HookContext& hookContext;
    ViewRenderHook& viewRenderHook;
    SoundWatcher<HookContext> soundWatcher;
};
