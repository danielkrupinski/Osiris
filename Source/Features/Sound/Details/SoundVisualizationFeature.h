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

template <typename SoundType>
struct SoundVisualizationFeatureToggle : FeatureToggleOnOff<SoundVisualizationFeatureToggle<SoundType>> {
    SoundVisualizationFeatureToggle(SoundVisualizationFeatureState& state,
        HookDependencies& hookDependencies,
        SoundWatcher soundWatcher,
        ViewRenderHook& viewRenderHook,
        HudInWorldPanelContainer& hudInWorldPanelContainer)
        : state{state}
        , hookDependencies{hookDependencies}
        , soundWatcher{soundWatcher}
        , viewRenderHook{viewRenderHook}
        , hudInWorldPanelContainer{hudInWorldPanelContainer}
    {
    }

    [[nodiscard]] auto& enabledVariable(typename SoundVisualizationFeatureToggle::ToggleMethod) const noexcept
    {
        return state.enabled;
    }

    void onEnable(typename SoundVisualizationFeatureToggle::ToggleMethod) noexcept
    {
        viewRenderHook.incrementReferenceCount();
        soundWatcher.startWatching<SoundType>();
    }

    void onDisable(typename SoundVisualizationFeatureToggle::ToggleMethod) noexcept
    {
        viewRenderHook.decrementReferenceCount();
        soundWatcher.stopWatching<SoundType>();
        if (const auto containerPanel{hudInWorldPanelContainer.get(hookDependencies.hud(), hookDependencies)}) {
            if (const auto containerPanelChildren{containerPanel.children().vector})
                state.hideRemainingPanels(hookDependencies, HudInWorldPanels{*containerPanelChildren}, 0);
        }
    }

private:
    SoundVisualizationFeatureState& state;
    HookDependencies& hookDependencies;
    SoundWatcher soundWatcher;
    ViewRenderHook& viewRenderHook;
    HudInWorldPanelContainer& hudInWorldPanelContainer;
};

template <typename PanelsType, typename SoundType>
class SoundVisualizationFeature {
public:
    SoundVisualizationFeature(
        SoundVisualizationFeatureState& state,
        HookDependencies& hookDependencies,
        ViewRenderHook& viewRenderHook,
        SoundWatcher soundWatcher,
        HudInWorldPanelContainer& hudInWorldPanelContainer) noexcept
        : state{state}
        , hookDependencies{hookDependencies}
        , viewRenderHook{viewRenderHook}
        , soundWatcher{soundWatcher}
        , hudInWorldPanelContainer{hudInWorldPanelContainer}
    {
    }

    void run() noexcept
    {
        if (!state.enabled)
            return;

        constexpr auto kCrucialDependencies{HookDependenciesMask{}.set<PanoramaTransformFactory>().set<WorldToClipSpaceConverter>()};
        if (!hookDependencies.requestDependencies(kCrucialDependencies))
            return;

        const auto curtime = hookDependencies.globalVars().curtime();
        if (!curtime)
            return;

        const auto containerPanel{hudInWorldPanelContainer.get(hookDependencies.hud(), hookDependencies)};
        if (!containerPanel)
            return;

        const auto containerPanelChildren{containerPanel.children().vector};
        if (!containerPanelChildren)
            return;

        const HudInWorldPanels panels{*containerPanelChildren};

        if (state.containerPanelHandle != PanoramaUiEngine::getPanelHandle(containerPanel)) {
            state.panelIndices.clear();
            state.containerPanelHandle = PanoramaUiEngine::getPanelHandle(containerPanel);
        }

        std::size_t currentIndex = 0;
        std::as_const(soundWatcher).getSoundsOfType<SoundType>().forEach([this, &currentIndex, containerPanel, panels, curtime](const PlayedSound& sound) {
            const auto soundInClipSpace = hookDependencies.getDependency<WorldToClipSpaceConverter>().toClipSpace(sound.origin);
            if (!soundInClipSpace.onScreen())
                return;

            const auto opacity = SoundVisualization<SoundType>::getOpacity(sound.getTimeAlive(*curtime));
            if (opacity <= 0.0f)
                return;

            const auto panel = getPanel(containerPanel, panels, currentIndex);
            if (!panel)
                return;

            panel.setOpacity(opacity);
            panel.setZIndex(-soundInClipSpace.z);

            const auto deviceCoordinates = soundInClipSpace.toNormalizedDeviceCoordinates();

            const auto& transformFactory = hookDependencies.getDependency<PanoramaTransformFactory>();
            PanoramaTransformations{
                transformFactory.scale(SoundVisualization<SoundType>::getScale(soundInClipSpace.z, ViewToProjectionMatrix{hookDependencies.gameDependencies().viewToProjectionMatrix}.getFovScale())),
                transformFactory.translate(deviceCoordinates.getX(), deviceCoordinates.getY())
            }.applyTo(panel);

            ++currentIndex;
        });

        state.hideRemainingPanels(hookDependencies, panels, currentIndex);
    }

private:
    [[nodiscard]] auto getPanel(auto&& containerPanel, HudInWorldPanels inWorldPanels, std::size_t index) const noexcept
    {
        if (index < state.panelIndices.getSize()) {
            if (const auto panel{inWorldPanels.getPanel(state.panelIndices[index], hookDependencies)})
                return panel;
            state.panelIndices.fastRemoveAt(index);
        }
        if (const auto panel{SoundVisualizationPanelFactory{hookDependencies, *static_cast<cs2::CUIPanel*>(containerPanel)}.createSoundVisualizationPanel(PanelsType::soundVisualizationPanelProperties())}) {
            state.panelIndices.pushBack(inWorldPanels.getIndexOfLastPanel());
            return panel;
        }
        return PanoramaUiPanel{PanoramaUiPanelContext{hookDependencies, nullptr}};
    }

    SoundVisualizationFeatureState& state;
    HookDependencies& hookDependencies;
    ViewRenderHook& viewRenderHook;
    SoundWatcher soundWatcher;
    HudInWorldPanelContainer& hudInWorldPanelContainer;
};
