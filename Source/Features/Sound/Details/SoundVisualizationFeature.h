#pragma once

#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <Hooks/ViewRenderHook.h>
#include "SoundVisualization.h"
#include <FeatureHelpers/Sound/SoundWatcher.h>

template <typename PanelsType>
struct SoundVisualizationFeatureState {
    bool enabled{false};

    HudInWorldPanels<PanelsType> panels;
};

template <typename PanelsType, typename SoundType>
class SoundVisualizationFeature : public TogglableFeature<SoundVisualizationFeature<PanelsType, SoundType>> {
public:
    SoundVisualizationFeature(
        SoundVisualizationFeatureState<PanelsType>& state,
        ViewRenderHook& viewRenderHook,
        SoundWatcher& soundWatcher,
        HudInWorldPanelFactory hudInWorldPanelFactory,
        GlobalVarsProvider globalVarsProvider,
        PanoramaTransformFactory transformFactory,
        WorldToClipSpaceConverter worldtoClipSpaceConverter,
        ViewToProjectionMatrix viewToProjectionMatrix,
        PanelConfigurator panelConfigurator) noexcept
        : SoundVisualizationFeature::TogglableFeature{state.enabled}
        , state{state}
        , viewRenderHook{viewRenderHook}
        , soundWatcher{soundWatcher}
        , hudInWorldPanelFactory{hudInWorldPanelFactory}
        , globalVarsProvider{globalVarsProvider}
        , transformFactory{transformFactory}
        , worldtoClipSpaceConverter{worldtoClipSpaceConverter}
        , viewToProjectionMatrix{viewToProjectionMatrix}
        , panelConfigurator{panelConfigurator}
    {
    }

    void run() noexcept
    {
        if (!this->isEnabled())
            return;

        if (!globalVarsProvider || !globalVarsProvider.getGlobalVars())
            return;

        if (!worldtoClipSpaceConverter)
            return;

        state.panels.createPanels(hudInWorldPanelFactory);

        std::size_t currentIndex = 0;
        std::as_const(soundWatcher).getSoundsOfType<SoundType>().forEach([this, &currentIndex](const PlayedSound& sound) {
            const auto soundInClipSpace = worldtoClipSpaceConverter.toClipSpace(sound.origin);
            if (!soundInClipSpace.onScreen())
                return;

            const auto opacity = SoundVisualization<SoundType>::getOpacity(sound.getTimeAlive(globalVarsProvider.getGlobalVars()->curtime));
            if (opacity <= 0.0f)
                return;

            const auto panel = state.panels.getPanel(currentIndex);
            if (!panel)
                return;

            const auto style = panel.getStyle();
            if (!style)
                return;

            const auto styleSetter{panelConfigurator.panelStyle(*style)};
            styleSetter.setOpacity(opacity);
            styleSetter.setZIndex(-soundInClipSpace.z);

            const auto deviceCoordinates = soundInClipSpace.toNormalizedDeviceCoordinates();

            PanoramaTransformations{
                transformFactory.scale(SoundVisualization<SoundType>::getScale(soundInClipSpace.z, viewToProjectionMatrix.getFovScale())),
                transformFactory.translate(deviceCoordinates.getX(), deviceCoordinates.getY())
            }.applyTo(styleSetter);

            ++currentIndex;
        });

        state.panels.hidePanels(currentIndex, panelConfigurator);
    }

private:
    friend TogglableFeature<SoundVisualizationFeature<PanelsType, SoundType>>;

    void onEnable() noexcept
    {
        viewRenderHook.incrementReferenceCount();
        soundWatcher.startWatching<SoundType>();
    }

    void onDisable() noexcept
    {
        viewRenderHook.decrementReferenceCount();
        soundWatcher.stopWatching<SoundType>();
        state.panels.hidePanels(0, panelConfigurator);
    }

    SoundVisualizationFeatureState<PanelsType>& state;
    ViewRenderHook& viewRenderHook;
    SoundWatcher& soundWatcher;
    HudInWorldPanelFactory hudInWorldPanelFactory;
    GlobalVarsProvider globalVarsProvider;
    PanoramaTransformFactory transformFactory;
    WorldToClipSpaceConverter worldtoClipSpaceConverter;
    ViewToProjectionMatrix viewToProjectionMatrix;
    PanelConfigurator panelConfigurator;
};
