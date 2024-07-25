#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <GameClasses/PanoramaImagePanelFactory.h>
#include <HookDependencies/HookDependencies.h>

class PlayerInformationThroughWallsPanelFactory {
public:
    PlayerInformationThroughWallsPanelFactory(cs2::CUIPanel& parentPanel, HookDependencies& dependencies) noexcept
        : parentPanel{parentPanel}
        , dependencies{dependencies}
    {
    }

    [[nodiscard]] auto createPlayerInformationPanel(PanoramaTransformFactory panoramaTransformFactory) const noexcept
    {
        const auto containerPanel{Panel::create("", &parentPanel)};
        if (!containerPanel)
            return PanoramaUiPanel{PanoramaUiPanelContext{dependencies, nullptr}};

        PanoramaUiPanel panel{PanoramaUiPanelContext{dependencies, containerPanel->uiPanel}};
        panel.setWidth(cs2::CUILength::pixels(kWidth));
        panel.setHeight(cs2::CUILength::pixels(kHeight));
        panel.setPosition(cs2::CUILength::pixels(-kWidth * 0.5f), cs2::CUILength::pixels(0.0f));
        panel.setTransformOrigin(cs2::CUILength::percent(50), cs2::CUILength::percent(0));
        panel.setFlowChildren(cs2::k_EFlowDown);

        createPositionArrowPanel(containerPanel->uiPanel, panoramaTransformFactory);
        createHealthPanel(containerPanel->uiPanel);
        createActiveWeaponIconPanel(containerPanel->uiPanel);
        createActiveWeaponAmmoPanel(containerPanel->uiPanel);
        createStateIconsPanel(containerPanel->uiPanel);
        return panel;
    }

private:
    void createActiveWeaponAmmoPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        const auto ammoPanel{Panel::create("", containerPanel)};
        if (!ammoPanel)
            return;

        PanoramaUiPanel panel{PanoramaUiPanelContext{dependencies, ammoPanel->uiPanel}};
        panel.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
        panel.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(3), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
        panel.setFlowChildren(cs2::k_EFlowRight);

        createActiveWeaponAmmoTextPanel(ammoPanel->uiPanel);
        createActiveWeaponAmmoIconPanel(ammoPanel->uiPanel);
    }

    void createActiveWeaponAmmoTextPanel(cs2::CUIPanel* containerPanel) const
    {
        if (!dependencies.requestDependency<PanoramaLabelFactory>())
            return;

        const auto label = dependencies.getDependency<PanoramaLabelFactory>().createLabelPanel(containerPanel);
        if (!label)
            return;

        PanoramaUiPanel panel{PanoramaUiPanelContext{dependencies, label->uiPanel}};
        panel.setFont("Stratum2, 'Arial Unicode MS'", 24.0f, cs2::k_EFontWeightBlack);
        panel.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        panel.setTextShadow(shadowParams());
        panel.setColor(cs2::kColorWhite);
    }

    void createActiveWeaponAmmoIconPanel(cs2::CUIPanel* containerPanel) const
    {
        const auto activeWeaponAmmoIconPanel = PanoramaImagePanelFactory::create("", containerPanel);
        if (!activeWeaponAmmoIconPanel)
            return;

        PanoramaImagePanel{PanoramaImagePanelContext{dependencies, activeWeaponAmmoIconPanel}}.setImageSvg("s2r://panorama/images/hud/bullet_single.vsvg", 20);
        PanoramaUiPanel panel{PanoramaUiPanelContext{dependencies, activeWeaponAmmoIconPanel->uiPanel}};
        panel.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        panel.setMargin(cs2::CUILength::pixels(5), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
        panel.setImageShadow(shadowParams());
    }

    void createStateIconsPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        const auto panel{Panel::create("", containerPanel)};
        if (!panel)
            return;

        PanoramaUiPanel _panel{PanoramaUiPanelContext{dependencies, panel->uiPanel}};
        _panel.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
        _panel.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(1), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
        _panel.setFlowChildren(cs2::k_EFlowRight);

        createDefuseIconPanel(panel->uiPanel);
        createHostagePickupPanel(panel->uiPanel);
        createHostageRescuePanel(panel->uiPanel);
        createBlindedIconPanel(panel->uiPanel);
    }

    void createDefuseIconPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        const auto imagePanel = PanoramaImagePanelFactory::create("", containerPanel);
        if (!imagePanel)
            return;

        PanoramaImagePanel{PanoramaImagePanelContext{dependencies, imagePanel}}.setImageSvg("s2r://panorama/images/icons/equipment/defuser.vsvg", 24);
        PanoramaUiPanel panel{PanoramaUiPanelContext{dependencies, imagePanel->uiPanel}};
        panel.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        panel.setImageShadow(shadowParams());
        panel.setWashColor(cs2::kColorDefuseKit);
    }

    void createHostagePickupPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        createHostageIconPanel(containerPanel, cs2::kColorYellow);
    }

    void createHostageRescuePanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        createHostageIconPanel(containerPanel, cs2::kColorLightGreen);
    }

    void createHostageIconPanel(cs2::CUIPanel* containerPanel, cs2::Color color) const noexcept
    {
        const auto imagePanel = PanoramaImagePanelFactory::create("", containerPanel);
        if (!imagePanel)
            return;

        PanoramaImagePanel{PanoramaImagePanelContext{dependencies, imagePanel}}.setImageSvg("s2r://panorama/images/icons/ui/hostage_transit.svg", 32);
        PanoramaUiPanel panel{PanoramaUiPanelContext{dependencies, imagePanel->uiPanel}};
        panel.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        panel.setImageShadow(shadowParams());
        panel.setWashColor(color);
    }

    void createBlindedIconPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        const auto imagePanel = PanoramaImagePanelFactory::create("", containerPanel);
        if (!imagePanel)
            return;

        PanoramaImagePanel{PanoramaImagePanelContext{dependencies, imagePanel}}.setImageSvg("s2r://panorama/images/hud/deathnotice/blind_kill.svg", 26);
        PanoramaUiPanel panel{PanoramaUiPanelContext{dependencies, imagePanel->uiPanel}};
        panel.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        panel.setImageShadow(shadowParams());
    }

    void createActiveWeaponIconPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        const auto weaponIconPanel = PanoramaImagePanelFactory::create("", containerPanel);
        if (!weaponIconPanel)
            return;

        PanoramaUiPanel panel{PanoramaUiPanelContext{dependencies, weaponIconPanel->uiPanel}};
        panel.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
        panel.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(3), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
        panel.setImageShadow(shadowParams());
    }

    void createHealthPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        const auto healthPanel{Panel::create("", containerPanel)};
        if (!healthPanel)
            return;

        PanoramaUiPanel panel{PanoramaUiPanelContext{dependencies, healthPanel->uiPanel}};
        panel.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
        panel.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(1), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
        panel.setFlowChildren(cs2::k_EFlowRight);

        createHealthIconPanel(healthPanel->uiPanel);
        createHealthTextPanel(healthPanel->uiPanel);
    }

    void createHealthIconPanel(cs2::CUIPanel* containerPanel) const
    {
        const auto healthIconPanel = PanoramaImagePanelFactory::create("", containerPanel);
        if (!healthIconPanel)
            return;

        PanoramaImagePanel{PanoramaImagePanelContext{dependencies, healthIconPanel}}.setImageSvg("s2r://panorama/images/hud/health_cross.vsvg", 24);

        PanoramaUiPanel panel{PanoramaUiPanelContext{dependencies, healthIconPanel->uiPanel}};
        panel.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        panel.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(0), cs2::CUILength::pixels(5), cs2::CUILength::pixels(0));
        panel.setImageShadow(shadowParams());
    }

    void createHealthTextPanel(cs2::CUIPanel* containerPanel) const
    {
        if (!dependencies.requestDependency<PanoramaLabelFactory>())
            return;

        const auto label = dependencies.getDependency<PanoramaLabelFactory>().createLabelPanel(containerPanel);
        if (!label)
            return;

        PanoramaUiPanel panel{PanoramaUiPanelContext{dependencies, label->uiPanel}};
        panel.setFont("Stratum2, 'Arial Unicode MS'", 24.0f, cs2::k_EFontWeightBlack);
        panel.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        panel.setTextShadow(shadowParams());
    }

    void createPositionArrowPanel(cs2::CUIPanel* containerPanel, PanoramaTransformFactory panoramaTransformFactory) const noexcept
    {
        const auto imagePanel = PanoramaImagePanelFactory::create("", containerPanel);
        if (!imagePanel)
            return;

        PanoramaImagePanel panel{PanoramaImagePanelContext{dependencies, imagePanel}};
        panel.setImageSvg("s2r://panorama/images/hud/reticle/playerid_arrow.vsvg", 24);

        auto&& uiPanel = panel.uiPanel();
        uiPanel.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
        uiPanel.setImageShadow(shadowParams());
        PanoramaTransformations{
            panoramaTransformFactory.scale(1.0f, -1.0f),
        }.applyTo(uiPanel);
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
};
