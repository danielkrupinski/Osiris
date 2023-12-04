#pragma once

#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <Hooks/ViewRenderHook.h>
#include "SoundVisualization.h"
#include "SoundVisualizationHelpers.h"
#include "SoundWatcher.h"

template <typename PanelsType, typename SoundType>
class SoundVisualizationFeature : public TogglableFeature<SoundVisualizationFeature<PanelsType, SoundType>> {
public:
    SoundVisualizationFeature(ViewRenderHook& viewRenderHook, SoundWatcher& soundWatcher) noexcept
        : viewRenderHook{ viewRenderHook }
        , soundWatcher{ soundWatcher }
    {
    }

    void run(const SoundVisualizationHelpers& params) noexcept
    {
        if (!this->isEnabled())
            return;

        if (!params.globalVarsProvider || !params.globalVarsProvider.getGlobalVars())
            return;

        if (!params.worldtoClipSpaceConverter)
            return;

        panels.createPanels(params.hudInWorldPanelFactory);

        std::size_t currentIndex = 0;
        std::as_const(soundWatcher).getSoundsOfType<SoundType>().forEach([this, &currentIndex, params](const PlayedSound& sound) {
            const auto soundInClipSpace = params.worldtoClipSpaceConverter.toClipSpace(sound.origin);
            if (!soundInClipSpace.onScreen())
                return;

            const auto opacity = SoundVisualization<SoundType>::getOpacity(sound.getTimeAlive(params.globalVarsProvider.getGlobalVars()->curtime));
            if (opacity <= 0.0f)
                return;

            const auto panel = panels.getPanel(currentIndex);
            if (!panel)
                return;

            const auto style = panel.getStyle();
            if (!style)
                return;

            style.setOpacity(opacity);
            style.setZIndex(-soundInClipSpace.z);

            const auto deviceCoordinates = soundInClipSpace.toNormalizedDeviceCoordinates();

            PanoramaTransformations{
                params.transformFactory.scale(SoundVisualization<SoundType>::getScale(soundInClipSpace.z, params.viewToProjectionMatrix.getFovScale())),
                params.transformFactory.translate(deviceCoordinates.getX(), deviceCoordinates.getY())
            }.applyTo(style);

            ++currentIndex;
        });

        panels.hidePanels(currentIndex);
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
        panels.hidePanels(0);
    }

    HudInWorldPanels<PanelsType> panels;
    ViewRenderHook& viewRenderHook;
    SoundWatcher& soundWatcher;
};
