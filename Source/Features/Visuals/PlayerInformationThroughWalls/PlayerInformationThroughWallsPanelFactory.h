#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <GameClasses/PanelFactory.h>
#include <HookDependencies/HookDependencies.h>

#include "ActiveWeaponAmmo/ActiveWeaponAmmoPanelParams.h"
#include "ActiveWeaponIcon/ActiveWeaponIconPanelParams.h"
#include "PlayerHealth/PlayerHealthPanelParams.h"
#include "PlayerInfoContainerPanelParams.h"
#include "PlayerPositionArrow/PlayerPositionArrowPanelParams.h"
#include "PlayerStateIcons/PlayerStateIconsPanelParams.h"

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
        auto&& containerPanel = createPlayerInfoContainerPanel();

        createPositionArrowPanel(containerPanel);
        createHealthPanel(containerPanel);
        createActiveWeaponIconPanel(containerPanel);
        createActiveWeaponAmmoPanel(containerPanel);
        createStateIconsPanel(containerPanel);
        return containerPanel;
    }

private:
    [[nodiscard]] decltype(auto) createPlayerInfoContainerPanel() const noexcept
    {
        using namespace player_info_container_panel_params;

        auto&& uiPanel = hookContext.panelFactory().createPanel(&parentPanel).uiPanel();
        uiPanel.setWidth(kWidth);
        uiPanel.setHeight(kHeight);
        uiPanel.setPosition(kPositionX, kPositionY);
        uiPanel.setTransformOrigin(kTransformOriginX, kTransformOriginY);
        uiPanel.setFlowChildren(kChildrenFlow);
        return utils::lvalue<decltype(uiPanel)>(uiPanel);
    }

    void createActiveWeaponAmmoPanel(auto&& containerPanel) const noexcept
    {
        auto&& ammoPanel = createActiveWeaponAmmoContainerPanel(containerPanel);
        createActiveWeaponAmmoTextPanel(ammoPanel);
        createActiveWeaponAmmoIconPanel(ammoPanel);
    }

    [[nodiscard]] decltype(auto) createActiveWeaponAmmoContainerPanel(auto&& containerPanel) const noexcept
    {
        using namespace active_weapon_ammo_panel_params::container_panel_params;

        auto&& uiPanel = hookContext.panelFactory().createPanel(containerPanel).uiPanel();
        uiPanel.setAlign(kHorizontalAlignment, kVerticalAlignment);
        uiPanel.setMargin(kMarginLeft, kMarginTop, kMarginRight, kMarginBottom);
        uiPanel.setFlowChildren(kChildrenFlow);
        return utils::lvalue<decltype(uiPanel)>(uiPanel);
    }

    void createActiveWeaponAmmoTextPanel(auto&& containerPanel) const
    {
        using namespace active_weapon_ammo_panel_params::ammo_text_panel_params;

        auto&& label = hookContext.panelFactory().createLabelPanel(containerPanel).uiPanel();
        label.setFont(kFontFamily, kFontSize, kFontWeight);
        label.setAlign(kHorizontalAlignment, kVerticalAlignment);
        label.setTextShadow(kShadowParams);
        label.setColor(kColor);
    }

    void createActiveWeaponAmmoIconPanel(auto&& containerPanel) const
    {
        using namespace active_weapon_ammo_panel_params::ammo_icon_panel_params;

        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(kImageUrl, kTextureHeight);
        
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kHorizontalAlignment, kVerticalAlignment);
        uiPanel.setMargin(kMarginLeft, kMarginTop, kMarginRight, kMarginBottom);
        uiPanel.setImageShadow(kShadowParams);
    }

    void createStateIconsPanel(auto&& containerPanel) const noexcept
    {
        using namespace player_state_icons_panel_params::container_panel_params;

        auto&& panel = hookContext.panelFactory().createPanel(containerPanel).uiPanel();
        panel.setAlign(kHorizontalAlignment, kVerticalAlignment);
        panel.setMargin(kMarginLeft, kMarginTop, kMarginRight, kMarginBottom);
        panel.setFlowChildren(kChildrenFlow);

        createDefuseIconPanel(panel);
        createHostagePickupPanel(panel);
        createHostageRescuePanel(panel);
        createBlindedIconPanel(panel);
    }

    void createDefuseIconPanel(auto&& containerPanel) const noexcept
    {
        using namespace player_state_icons_panel_params::defuse_icon_panel_params;

        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(kImageUrl, kTextureHeight);
    
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kHorizontalAlignment, kVerticalAlignment);
        uiPanel.setImageShadow(kShadowParams);
        uiPanel.setWashColor(kWashColor);
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
        using namespace player_state_icons_panel_params::hostage_icon_panel_params;

        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(kImageUrl, kTextureHeight);
    
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kHorizontalAlignment, kVerticalAlignment);
        uiPanel.setImageShadow(kShadowParams);
        uiPanel.setWashColor(color);
    }

    void createBlindedIconPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        using namespace player_state_icons_panel_params::blinded_icon_panel_params;

        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(kImageUrl, kTextureHeight);

        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kHorizontalAlignment, kVerticalAlignment);
        uiPanel.setImageShadow(kShadowParams);
    }

    void createActiveWeaponIconPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        using namespace active_weapon_icon_panel_params;

        auto&& panel = hookContext.panelFactory().createImagePanel(containerPanel).uiPanel();
        panel.setAlign(kHorizontalAlignment, kVerticalAlignment);
        panel.setMargin(kMarginLeft, kMarginTop, kMarginRight, kMarginBottom);
        panel.setImageShadow(kShadowParams);
    }

    void createHealthPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        using namespace player_health_panel_params::container_panel_params;

        auto&& healthPanel = hookContext.panelFactory().createPanel(containerPanel).uiPanel();
        healthPanel.setAlign(kHorizontalAlignment, kVerticalAlignment);
        healthPanel.setMargin(kMarginLeft, kMarginTop, kMarginRight, kMarginBottom);
        healthPanel.setFlowChildren(kChildrenFlow);

        createHealthIconPanel(healthPanel);
        createHealthTextPanel(healthPanel);
    }

    void createHealthIconPanel(cs2::CUIPanel* containerPanel) const
    {
        using namespace player_health_panel_params::health_icon_panel_params;

        auto&& healthIconPanel = hookContext.panelFactory().createImagePanel(containerPanel);
        healthIconPanel.setImageSvg(kImageUrl, kTextureHeight);

        auto&& uiPanel = healthIconPanel.uiPanel();
        uiPanel.setAlign(kHorizontalAlignment, kVerticalAlignment);
        uiPanel.setMargin(kMarginLeft, kMarginTop, kMarginRight, kMarginBottom);
        uiPanel.setImageShadow(kShadowParams);
    }

    void createHealthTextPanel(cs2::CUIPanel* containerPanel) const
    {
        using namespace player_health_panel_params::health_text_panel_params;

        auto&& label = hookContext.panelFactory().createLabelPanel(containerPanel).uiPanel();
        label.setFont(kFontFamily, kFontSize, kFontWeight);
        label.setAlign(kHorizontalAlignment, kVerticalAlignment);
        label.setTextShadow(kShadowParams);
    }

    void createPositionArrowPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        using namespace player_position_arrow_panel_params;

        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(kImageUrl, kTextureHeight);

        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kHorizontalAlignment, kVerticalAlignment);
        uiPanel.setImageShadow(kShadowParams);
        PanoramaTransformations{
            hookContext.panoramaTransformFactory().scale(1.0f, -1.0f),
        }.applyTo(uiPanel);
    }

    HookContext& hookContext;
    cs2::CUIPanel& parentPanel;
};
