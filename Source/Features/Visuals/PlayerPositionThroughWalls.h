#pragma once

#include <CS2/Classes/CGameSceneNode.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/CCSPlayerController.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/TeamNumber.h>
#include <MemoryPatterns/EntityPatterns.h>
#include <MemoryPatterns/PlayerControllerPatterns.h>

#include "States/PlayerPositionThroughWallsState.h"

#include <HookDependencies/HookDependencies.h>
#include <HookDependencies/HookDependenciesMask.h>

class PlayerESPPanelFactory {
public:
    PlayerESPPanelFactory(cs2::CUIPanel& parentPanel, PanelConfigurator panelConfigurator) noexcept
        : parentPanel{parentPanel}
        , panelConfigurator{panelConfigurator}
    {
    }

    [[nodiscard]] PanoramaUiPanel createPanel(PanoramaTransformFactory panoramaTransformFactory) const noexcept
    {
        const auto containerPanel{Panel::create("", &parentPanel)};
        if (!containerPanel)
            return PanoramaUiPanel{nullptr};

        if (const auto style{PanoramaUiPanel{containerPanel->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setWidth(cs2::CUILength::pixels(kWidth));
            styler.setHeight(cs2::CUILength::pixels(kHeight));
            styler.setPosition(cs2::CUILength::pixels(-kWidth * 0.5f), cs2::CUILength::pixels(0.0f));
            styler.setTransformOrigin(cs2::CUILength::percent(50), cs2::CUILength::percent(0));
        }

        applyStyleToImagePanel(PanoramaImagePanel::create("", containerPanel->uiPanel), panoramaTransformFactory);
        return PanoramaUiPanel{containerPanel->uiPanel};
    }

private:
    void applyStyleToImagePanel(cs2::CImagePanel* imagePanel, PanoramaTransformFactory panoramaTransformFactory) const noexcept
    {
        if (!imagePanel)
            return;

        PanoramaImagePanel{imagePanel}.setImageSvg("s2r://panorama/images/hud/reticle/playerid_arrow.vsvg", 32);
        if (const auto style{PanoramaUiPanel{imagePanel->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
            styler.setImageShadow(imageShadowParams());
            PanoramaTransformations{
                panoramaTransformFactory.scale(1.0f, -1.0f),
            }.applyTo(styler);
        }
    }

    [[nodiscard]] static ImageShadowParams imageShadowParams() noexcept
    {
        return ImageShadowParams{
            .horizontalOffset{cs2::CUILength::pixels(0)},
            .verticalOffset{cs2::CUILength::pixels(0)},
            .blurRadius{cs2::CUILength::pixels(1)},
            .strength = 3,
            .color{0, 0, 0}
        };
    }

    static constexpr auto kWidth{256};
    static constexpr auto kHeight{256};

    cs2::CUIPanel& parentPanel;
    PanelConfigurator panelConfigurator;
};

struct PlayerPositionThroughWallsToggle : public TogglableFeature<PlayerPositionThroughWallsToggle> {
    PlayerPositionThroughWallsToggle(PlayerPositionThroughWallsState& state, HudInWorldPanelContainer& hudInWorldPanelContainer, ViewRenderHook& viewRenderHook, PanelConfigurator panelConfigurator, HudProvider hudProvider) noexcept
        : TogglableFeature{state.enabled}
        , state{state}
        , hudInWorldPanelContainer{hudInWorldPanelContainer}
        , viewRenderHook{viewRenderHook}
        , panelConfigurator{panelConfigurator}
        , hudProvider{hudProvider}
    {
    }

private:
    friend TogglableFeature;

    void onEnable() noexcept
    {
        viewRenderHook.incrementReferenceCount();
    }

    void onDisable() noexcept
    {
        viewRenderHook.decrementReferenceCount();

        if (const auto containerPanel{hudInWorldPanelContainer.get(hudProvider, panelConfigurator)}) {
            if (const auto containerPanelChildren{containerPanel.children()})
                hideRemainingPanels(HudInWorldPanels{*containerPanelChildren}, 0);
        }
    }

    void hideRemainingPanels(HudInWorldPanels inWorldPanels, std::size_t firstIndexToHide) const noexcept
    {
        for (std::size_t i = firstIndexToHide; i < state.panelIndices.getSize(); ++i) {
            if (const auto panel{inWorldPanels.getPanel(state.panelIndices[i])}) {
                if (const auto style{panel.getStyle()})
                    panelConfigurator.panelStyle(*style).setOpacity(0.0f);
            }
        }
    }

    PlayerPositionThroughWallsState& state;
    HudInWorldPanelContainer& hudInWorldPanelContainer;
    ViewRenderHook& viewRenderHook;
    PanelConfigurator panelConfigurator;
    HudProvider hudProvider;
};

class PlayerPositionThroughWalls {
public:
    PlayerPositionThroughWalls(PlayerPositionThroughWallsState& state, HookDependencies& dependencies) noexcept
        : state{state}
        , dependencies{dependencies}
    {
    }

    void run(cs2::C_CSPlayerPawn& nonLocalPlayerPawn) noexcept
    {
        if (!state.enabled)
            return;

        if (!requestCrucialDependencies())
            return;

        const auto teamNumber = getTeamNumber(nonLocalPlayerPawn);
        if (teamNumber != TeamNumber::TT && teamNumber != TeamNumber::CT)
            return;

        const auto gameSceneNode = *dependencies.getDependency<OffsetToGameSceneNode>().of(&nonLocalPlayerPawn).get();
        if (!gameSceneNode)
            return;

        const auto absOrigin = *dependencies.getDependency<OffsetToAbsOrigin>().of(gameSceneNode).get();

        const auto soundInClipSpace = dependencies.getDependency<WorldToClipSpaceConverter>().toClipSpace(absOrigin);
        if (!soundInClipSpace.onScreen())
            return;

        const auto containerPanel{dependencies.getDependency<HudInWorldPanelContainer>().get(dependencies.getDependency<HudProvider>(), dependencies.getDependency<PanelConfigurator>())};
        if (!containerPanel)
            return;

        const auto containerPanelChildren{containerPanel.children()};
        if (!containerPanelChildren)
            return;

        const HudInWorldPanels panels{*containerPanelChildren};

        if (state.containerPanelHandle != PanoramaUiEngine::getPanelHandle(containerPanel)) {
            state.panelIndices.clear();
            state.containerPanelHandle = PanoramaUiEngine::getPanelHandle(containerPanel);
        }

        const auto panel = getPanel(containerPanel, panels, dependencies.getDependency<PanelConfigurator>());
        if (!panel)
            return;

        setArrowColor(panel, getArrowColor(teamNumber));

        const auto style = panel.getStyle();
        if (!style)
            return;

        const auto styleSetter{dependencies.getDependency<PanelConfigurator>().panelStyle(*style)};
        styleSetter.setOpacity(1.0f);
        styleSetter.setZIndex(-soundInClipSpace.z);

        const auto deviceCoordinates = soundInClipSpace.toNormalizedDeviceCoordinates();

        constexpr auto kMaxScale{1.0f};
        const auto scale = std::clamp(500.0f / (soundInClipSpace.z / getFovScale() + 400.0f), 0.3f, kMaxScale);

        PanoramaTransformations{
            dependencies.getDependency<PanoramaTransformFactory>().scale(scale),
            dependencies.getDependency<PanoramaTransformFactory>().translate(deviceCoordinates.getX(), deviceCoordinates.getY())
        }.applyTo(styleSetter);

        ++currentIndex;
    }

    void hideUnusedPanels() const noexcept
    {
        if (!requestCrucialDependencies())
            return;

        const auto containerPanel{dependencies.getDependency<HudInWorldPanelContainer>().get(dependencies.getDependency<HudProvider>(), dependencies.getDependency<PanelConfigurator>())};
        if (!containerPanel)
            return;

        const auto containerPanelChildren{containerPanel.children()};
        if (!containerPanelChildren)
            return;

        const HudInWorldPanels panels{*containerPanelChildren};

        for (std::size_t i = currentIndex; i < state.panelIndices.getSize(); ++i) {
            if (const auto panel{panels.getPanel(state.panelIndices[i])}) {
                if (const auto style{panel.getStyle()})
                    dependencies.getDependency<PanelConfigurator>().panelStyle(*style).setOpacity(0.0f);
            }
        }
    }

private:
    void setArrowColor(PanoramaUiPanel parentPanel, cs2::Color color) const noexcept
    {
        const auto children = parentPanel.children();
        if (!children || children->size < 1)
            return;

        const PanoramaUiPanel arrowPanel{children->memory[0]};
        const auto style = arrowPanel.getStyle();
        if (!style)
            return;

        const auto styleSetter{dependencies.getDependency<PanelConfigurator>().panelStyle(*style)};
        styleSetter.setWashColor(color);
    }

    [[nodiscard]] TeamNumber getTeamNumber(cs2::C_BaseEntity& entity) const noexcept
    {
        if (dependencies.requestDependency<OffsetToTeamNumber>())
            return TeamNumber{*dependencies.getDependency<OffsetToTeamNumber>().of(&entity).get()};
        return {};
    }

    [[nodiscard]] static cs2::Color getArrowColor(TeamNumber teamNumber) noexcept
    {
        switch (teamNumber) {
            using enum TeamNumber;
            case TT: return cs2::Color{234, 190, 84};
            case CT: return cs2::Color{150, 200, 250};
            default: return cs2::Color{255, 255, 255};
        }
    }

    [[nodiscard]] bool requestCrucialDependencies() const noexcept
    {
        return dependencies.requestDependencies(kCrucialDependencies);
    }

    [[nodiscard]] float getFovScale() const noexcept
    {
        if (dependencies.requestDependency<FovScale>())
            return dependencies.getDependency<FovScale>();
        return 1.0f;
    }

    [[nodiscard]] PanoramaUiPanel getPanel(PanoramaUiPanel containerPanel, HudInWorldPanels inWorldPanels, PanelConfigurator panelConfigurator) const noexcept
    {
        if (currentIndex < state.panelIndices.getSize()) {
            if (const auto panel{inWorldPanels.getPanel(state.panelIndices[currentIndex])})
                return panel;
            state.panelIndices.fastRemoveAt(currentIndex);
        }
        if (const auto panel{PlayerESPPanelFactory{*static_cast<cs2::CUIPanel*>(containerPanel), panelConfigurator}.createPanel(dependencies.getDependency<PanoramaTransformFactory>())}) {
            state.panelIndices.pushBack(inWorldPanels.getIndexOfLastPanel());
            return panel;
        }
        return PanoramaUiPanel{nullptr};
    }

    static constexpr auto kCrucialDependencies{
        HookDependenciesMask{}
            .set<OffsetToGameSceneNode>()
            .set<OffsetToAbsOrigin>()
            .set<WorldToClipSpaceConverter>()
            .set<HudInWorldPanelContainer>()
            .set<HudProvider>()
            .set<PanelConfigurator>()
            .set<PanoramaTransformFactory>()};

    PlayerPositionThroughWallsState& state;
    HookDependencies& dependencies;
    std::size_t currentIndex{0};
};
