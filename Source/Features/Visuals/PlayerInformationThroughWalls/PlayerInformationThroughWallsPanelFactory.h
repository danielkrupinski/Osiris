#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <GameClasses/PanelFactory.h>
#include <HookDependencies/HookDependencies.h>

template <typename HookContext>
class PlayerInformationThroughWallsPanelFactory {
public:
    PlayerInformationThroughWallsPanelFactory(HookContext& hookContext, cs2::CUIPanel& parentPanel) noexcept
        : hookContext{hookContext}
        , parentPanel{parentPanel}
    {
    }

    [[nodiscard]] auto createPlayerInformationPanel() const noexcept
    {
        auto&& uiPanel = hookContext.panelFactory().createPanel(&parentPanel).uiPanel();
        uiPanel.setWidth(cs2::CUILength::pixels(kWidth));
        uiPanel.setHeight(cs2::CUILength::pixels(kHeight));
        uiPanel.setPosition(cs2::CUILength::pixels(-kWidth * 0.5f), cs2::CUILength::pixels(0.0f));
        uiPanel.setTransformOrigin(cs2::CUILength::percent(50), cs2::CUILength::percent(0));
        uiPanel.setFlowChildren(cs2::k_EFlowDown);

        createPositionArrowPanel(uiPanel);
        createHealthPanel(uiPanel);
        createActiveWeaponIconPanel(uiPanel);
        createActiveWeaponAmmoPanel(uiPanel);
        createStateIconsPanel(uiPanel);
        return uiPanel;
    }

private:
    void createActiveWeaponAmmoPanel(auto&& containerPanel) const noexcept
    {
        auto&& ammoPanel = hookContext.panelFactory().createPanel(containerPanel).uiPanel();
        ammoPanel.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
        ammoPanel.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(3), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
        ammoPanel.setFlowChildren(cs2::k_EFlowRight);

        createActiveWeaponAmmoTextPanel(ammoPanel);
        createActiveWeaponAmmoIconPanel(ammoPanel);
    }

    void createActiveWeaponAmmoTextPanel(auto&& containerPanel) const
    {
        auto&& label = hookContext.panelFactory().createLabelPanel(containerPanel).uiPanel();
        label.setFont("Stratum2, 'Arial Unicode MS'", 24.0f, cs2::k_EFontWeightBlack);
        label.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        label.setTextShadow(shadowParams());
        label.setColor(cs2::kColorWhite);
    }

    void createActiveWeaponAmmoIconPanel(auto&& containerPanel) const
    {
        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg("s2r://panorama/images/hud/bullet_single.vsvg", 20);
        
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        uiPanel.setMargin(cs2::CUILength::pixels(5), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
        uiPanel.setImageShadow(shadowParams());
    }

    void createStateIconsPanel(auto&& containerPanel) const noexcept
    {
        auto&& panel = hookContext.panelFactory().createPanel(containerPanel).uiPanel();
        panel.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
        panel.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(1), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
        panel.setFlowChildren(cs2::k_EFlowRight);

        createDefuseIconPanel(panel);
        createHostagePickupPanel(panel);
        createHostageRescuePanel(panel);
        createBlindedIconPanel(panel);
    }

    void createDefuseIconPanel(auto&& containerPanel) const noexcept
    {
        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg("s2r://panorama/images/icons/equipment/defuser.vsvg", 24);
    
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        uiPanel.setImageShadow(shadowParams());
        uiPanel.setWashColor(cs2::kColorDefuseKit);
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
        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg("s2r://panorama/images/icons/ui/hostage_transit.svg", 32);
    
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        uiPanel.setImageShadow(shadowParams());
        uiPanel.setWashColor(color);
    }

    void createBlindedIconPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg("s2r://panorama/images/hud/deathnotice/blind_kill.svg", 26);

        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        uiPanel.setImageShadow(shadowParams());
    }

    void createActiveWeaponIconPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        auto&& panel = hookContext.panelFactory().createImagePanel(containerPanel).uiPanel();
        panel.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
        panel.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(3), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
        panel.setImageShadow(shadowParams());
    }

    void createHealthPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        auto&& healthPanel = hookContext.panelFactory().createPanel(containerPanel).uiPanel();
        healthPanel.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
        healthPanel.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(1), cs2::CUILength::pixels(0), cs2::CUILength::pixels(0));
        healthPanel.setFlowChildren(cs2::k_EFlowRight);

        createHealthIconPanel(healthPanel);
        createHealthTextPanel(healthPanel);
    }

    void createHealthIconPanel(cs2::CUIPanel* containerPanel) const
    {
        auto&& healthIconPanel = hookContext.panelFactory().createImagePanel(containerPanel);
        healthIconPanel.setImageSvg("s2r://panorama/images/hud/health_cross.vsvg", 24);

        auto&& uiPanel = healthIconPanel.uiPanel();
        uiPanel.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        uiPanel.setMargin(cs2::CUILength::pixels(0), cs2::CUILength::pixels(0), cs2::CUILength::pixels(5), cs2::CUILength::pixels(0));
        uiPanel.setImageShadow(shadowParams());
    }

    void createHealthTextPanel(cs2::CUIPanel* containerPanel) const
    {
        auto&& label = hookContext.panelFactory().createLabelPanel(containerPanel).uiPanel();
        label.setFont("Stratum2, 'Arial Unicode MS'", 24.0f, cs2::k_EFontWeightBlack);
        label.setAlign(cs2::k_EHorizontalAlignmentUnset, cs2::k_EVerticalAlignmentCenter);
        label.setTextShadow(shadowParams());
    }

    void createPositionArrowPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg("s2r://panorama/images/hud/reticle/playerid_arrow.vsvg", 24);

        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(cs2::k_EHorizontalAlignmentCenter, cs2::k_EVerticalAlignmentTop);
        uiPanel.setImageShadow(shadowParams());
        PanoramaTransformations{
            hookContext.panoramaTransformFactory().scale(1.0f, -1.0f),
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

    HookContext& hookContext;
    cs2::CUIPanel& parentPanel;
};
