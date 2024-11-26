#pragma once

#include <FeatureHelpers/HudInWorldPanelIndex.h>
#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/PanoramaTransformations.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <FeatureHelpers/FeatureToggle.h>
#include <Hooks/ViewRenderHook.h>
#include "SoundVisualization.h"
#include "SoundVisualizationPanelFactory.h"

struct SoundVisualizationFeatureState {
    bool enabled{false};

    cs2::PanelHandle containerPanelHandle;
    DynamicArray<HudInWorldPanelIndex> panelIndices;

    void hideRemainingPanels(auto& hookContext, HudInWorldPanels inWorldPanels, std::size_t firstIndexToHide) const noexcept
    {
        for (std::size_t i = firstIndexToHide; i < panelIndices.getSize(); ++i) {
            if (const auto panel{inWorldPanels.getPanel(panelIndices[i], hookContext)}) {
                panel.setOpacity(0.0f);
            }
        }
    }
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
        if (const auto containerPanel{hookContext.template make<InWorldPanelContainer>().get()}) {
            if (const auto containerPanelChildren{containerPanel.children().vector})
                state.hideRemainingPanels(hookContext, HudInWorldPanels{*containerPanelChildren}, 0);
        }
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

        const auto containerPanel{hookContext.template make<InWorldPanelContainer>().get()};
        if (!containerPanel)
            return;

        const auto containerPanelChildren{containerPanel.children().vector};
        if (!containerPanelChildren)
            return;

        const HudInWorldPanels panels{*containerPanelChildren};

        if (state.containerPanelHandle != hookContext.template make<PanoramaUiEngine>().getPanelHandle(containerPanel)) {
            state.panelIndices.clear();
            state.containerPanelHandle = hookContext.template make<PanoramaUiEngine>().getPanelHandle(containerPanel);
        }

        std::size_t currentIndex = 0;
        std::as_const(soundWatcher).template getSoundsOfType<SoundType>().forEach([this, &currentIndex, containerPanel, panels, curtime](const PlayedSound& sound) {
            const auto soundInClipSpace = hookContext.template make<WorldToClipSpaceConverter>().toClipSpace(sound.origin);
            if (!soundInClipSpace.onScreen())
                return;

            const auto opacity = SoundVisualization<SoundType>::getOpacity(sound.getTimeAlive(curtime.value()));
            if (opacity <= 0.0f)
                return;

            const auto panel = getPanel(containerPanel, panels, currentIndex);
            if (!panel)
                return;

            panel.setOpacity(opacity);
            panel.setZIndex(-soundInClipSpace.z);

            const auto deviceCoordinates = soundInClipSpace.toNormalizedDeviceCoordinates();

            auto&& transformFactory = hookContext.panoramaTransformFactory();
            PanoramaTransformations{
                transformFactory.scale(SoundVisualization<SoundType>::getScale(soundInClipSpace.z, ViewToProjectionMatrix{hookContext.clientPatternSearchResults().template get<ViewToProjectionMatrixPointer>()}.getFovScale())),
                transformFactory.translate(deviceCoordinates.getX(), deviceCoordinates.getY())
            }.applyTo(panel);

            ++currentIndex;
        });

        state.hideRemainingPanels(hookContext, panels, currentIndex);
    }

private:
    [[nodiscard]] auto getPanel(auto&& containerPanel, HudInWorldPanels inWorldPanels, std::size_t index) const noexcept
    {
        if (index < state.panelIndices.getSize()) {
            if (const auto panel{inWorldPanels.getPanel(state.panelIndices[index], hookContext)})
                return panel;
            state.panelIndices.fastRemoveAt(index);
        }
        if (const auto panel{SoundVisualizationPanelFactory{hookContext, *static_cast<cs2::CUIPanel*>(containerPanel)}.createSoundVisualizationPanel(PanelsType::soundVisualizationPanelProperties())}) {
            state.panelIndices.pushBack(inWorldPanels.getIndexOfLastPanel());
            return panel;
        }
        return PanoramaUiPanel{PanoramaUiPanelContext{hookContext, nullptr}};
    }

    SoundVisualizationFeatureState& state;
    HookContext& hookContext;
    ViewRenderHook& viewRenderHook;
    SoundWatcher<HookContext> soundWatcher;
};
