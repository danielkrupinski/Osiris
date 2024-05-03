#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <FeatureHelpers/PanelConfigurator.h>
#include <HookDependencies/HookDependencies.h>

class PlayerInformationThroughWallsPanelFactory {
public:
    PlayerInformationThroughWallsPanelFactory(cs2::CUIPanel& parentPanel, HookDependencies& dependencies, PanelConfigurator panelConfigurator) noexcept
        : parentPanel{parentPanel}
        , dependencies{dependencies}
        , panelConfigurator{panelConfigurator}
    {
    }

    [[nodiscard]] PanoramaUiPanel createPlayerInformationPanel(PanoramaTransformFactory panoramaTransformFactory) const noexcept
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
            styler.setFlowChildren(cs2::k_EFlowDown);
        }

        createPositionArrowPanel(containerPanel->uiPanel, panoramaTransformFactory);
        createHealthPanel(containerPanel->uiPanel);
        createActiveWeaponIconPanel(containerPanel->uiPanel);
        return PanoramaUiPanel{containerPanel->uiPanel};
    }

private:
    void createActiveWeaponIconPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        const auto weaponIconPanel = PanoramaImagePanel::create("", containerPanel);
        if (!weaponIconPanel)
            return;

        if (const auto style{PanoramaUiPanel{weaponIconPanel->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
            styler.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(3), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
            styler.setImageShadow(shadowParams());
        }
    }

    void createHealthPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        const auto healthPanel{Panel::create("", containerPanel)};
        if (!healthPanel)
            return;

        if (const auto style{PanoramaUiPanel{healthPanel->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
            styler.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(1), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
            styler.setFlowChildren(cs2::k_EFlowRight);
        }

        createHealthIconPanel(healthPanel->uiPanel);
        createHealthTextPanel(healthPanel->uiPanel);
    }

    void createHealthIconPanel(cs2::CUIPanel* containerPanel) const
    {
        const auto healthIconPanel = PanoramaImagePanel::create("", containerPanel);
        if (!healthIconPanel)
            return;

        PanoramaImagePanel{healthIconPanel}.setImageSvg("s2r://panorama/images/hud/health_cross.vsvg", 24);
        if (const auto style{PanoramaUiPanel{healthIconPanel->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
            styler.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(0), cs2::CUILength::pixels(5), cs2::CUILength::pixels(0));
            styler.setImageShadow(shadowParams());
        }
    }

    void createHealthTextPanel(cs2::CUIPanel* containerPanel) const
    {
        if (!dependencies.requestDependency<PanoramaLabelFactory>())
            return;

        const auto label = dependencies.getDependency<PanoramaLabelFactory>().createLabelPanel(containerPanel);
        if (!label)
            return;

        if (const auto style{PanoramaUiPanel{label->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setFont("Stratum2, 'Arial Unicode MS'", 24.0f, cs2::k_EFontWeightBlack);
            styler.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
            styler.setTextShadow(shadowParams());
        }
    }

    void createPositionArrowPanel(cs2::CUIPanel* containerPanel, PanoramaTransformFactory panoramaTransformFactory) const noexcept
    {
        const auto imagePanel = PanoramaImagePanel::create("", containerPanel);
        if (!imagePanel)
            return;

        PanoramaImagePanel{imagePanel}.setImageSvg("s2r://panorama/images/hud/reticle/playerid_arrow.vsvg", 24);
        if (const auto style{PanoramaUiPanel{imagePanel->uiPanel}.getStyle()}) {
            const auto styler{panelConfigurator.panelStyle(*style)};
            styler.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
            styler.setImageShadow(shadowParams());
            PanoramaTransformations{
                panoramaTransformFactory.scale(1.0f, -1.0f),
            }.applyTo(styler);
        }
    }

    [[nodiscard]] static PanelShadowParams shadowParams() noexcept
    {
        return PanelShadowParams{
            .horizontalOffset{cs2::CUILength::pixels(0)},
            .verticalOffset{cs2::CUILength::pixels(0)},
            .blurRadius{cs2::CUILength::pixels(3)},
            .strength = 3,
            .color{cs2::kColorBlack}
        };
    }

    static constexpr auto kWidth{256};
    static constexpr auto kHeight{256};

    cs2::CUIPanel& parentPanel;
    HookDependencies& dependencies;
    PanelConfigurator panelConfigurator;
};
