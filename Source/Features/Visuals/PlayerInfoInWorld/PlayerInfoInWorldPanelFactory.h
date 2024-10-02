#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <GameClasses/PanelFactory.h>

#include "ActiveWeaponAmmo/ActiveWeaponAmmoPanelParams.h"
#include "ActiveWeaponIcon/ActiveWeaponIconPanelParams.h"
#include "PlayerHealth/PlayerHealthPanelParams.h"
#include "PlayerInfoContainerPanelParams.h"
#include "PlayerPositionArrow/PlayerPositionArrowPanelParams.h"
#include "PlayerStateIcons/PlayerStateIconsPanelParams.h"

template <typename HookContext>
class PlayerInfoInWorldPanelFactory {
public:
    PlayerInfoInWorldPanelFactory(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) createContainerPanel(auto&& parentPanel) const noexcept
    {
        using namespace player_info_container_panel_params;

        auto&& uiPanel = hookContext.panelFactory().createPanel(parentPanel).uiPanel();
        uiPanel.setWidth(kWidth);
        uiPanel.setHeight(kHeight);
        uiPanel.setPosition(kPositionX, kPositionY);
        uiPanel.setTransformOrigin(kTransformOriginX, kTransformOriginY);
        uiPanel.setFlowChildren(kChildrenFlow);
        return utils::lvalue<decltype(uiPanel)>(uiPanel);
    }

    void createPanel(std::type_identity<PlayerPositionArrowPanel<HookContext>>, auto&& containerPanel) const noexcept
    {
        using namespace player_position_arrow_panel_params;

        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(kImageUrl, kTextureHeight);

        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kAlignment);
        uiPanel.setImageShadow(kShadowParams);
        PanoramaTransformations{
            hookContext.panoramaTransformFactory().scale(1.0f, -1.0f),
        }.applyTo(uiPanel);
    }

    void createPanel(std::type_identity<PlayerActiveWeaponAmmoPanel<HookContext>>, auto&& containerPanel) const noexcept
    {
        auto&& ammoPanel = createActiveWeaponAmmoContainerPanel(containerPanel);
        createActiveWeaponAmmoTextPanel(ammoPanel);
        createActiveWeaponAmmoIconPanel(ammoPanel);
    }

    void createPanel(std::type_identity<PlayerStateIconsPanel<HookContext>>, auto&& containerPanel) const noexcept
    {
        using namespace player_state_icons_panel_params::container_panel_params;

        auto&& panel = hookContext.panelFactory().createPanel(containerPanel).uiPanel();
        panel.setAlign(kAlignment);
        panel.setMargin(kMargin);
        panel.setFlowChildren(kChildrenFlow);

        createDefuseIconPanel(panel);
        createHostagePickupPanel(panel);
        createHostageRescuePanel(panel);
        createBlindedIconPanel(panel);
        createBombIconPanel(panel);
    }

    void createPanel(std::type_identity<PlayerActiveWeaponIconPanel<HookContext>>, cs2::CUIPanel* containerPanel) const noexcept
    {
        using namespace active_weapon_icon_panel_params;

        auto&& panel = hookContext.panelFactory().createImagePanel(containerPanel).uiPanel();
        panel.setAlign(kAlignment);
        panel.setMargin(kMargin);
        panel.setImageShadow(kShadowParams);
    }

    void createPanel(std::type_identity<PlayerHealthPanel<HookContext>>, cs2::CUIPanel* containerPanel) const noexcept
    {
        using namespace player_health_panel_params::container_panel_params;

        auto&& healthPanel = hookContext.panelFactory().createPanel(containerPanel).uiPanel();
        healthPanel.setAlign(kAlignment);
        healthPanel.setMargin(kMargin);
        healthPanel.setFlowChildren(kChildrenFlow);

        createHealthIconPanel(healthPanel);
        createHealthTextPanel(healthPanel);
    }

private:
    [[nodiscard]] decltype(auto) createActiveWeaponAmmoContainerPanel(auto&& containerPanel) const noexcept
    {
        using namespace active_weapon_ammo_panel_params::container_panel_params;

        auto&& uiPanel = hookContext.panelFactory().createPanel(containerPanel).uiPanel();
        uiPanel.setAlign(kAlignment);
        uiPanel.setMargin(kMargin);
        uiPanel.setFlowChildren(kChildrenFlow);
        return utils::lvalue<decltype(uiPanel)>(uiPanel);
    }

    void createActiveWeaponAmmoTextPanel(auto&& containerPanel) const
    {
        using namespace active_weapon_ammo_panel_params::ammo_text_panel_params;

        auto&& label = hookContext.panelFactory().createLabelPanel(containerPanel).uiPanel();
        label.setFont(kFont);
        label.setAlign(kAlignment);
        label.setTextShadow(kShadowParams);
        label.setColor(kColor);
    }

    void createActiveWeaponAmmoIconPanel(auto&& containerPanel) const
    {
        using namespace active_weapon_ammo_panel_params::ammo_icon_panel_params;

        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(kImageUrl, kTextureHeight);
        
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kAlignment);
        uiPanel.setMargin(kMargin);
        uiPanel.setImageShadow(kShadowParams);
    }

    void createDefuseIconPanel(auto&& containerPanel) const noexcept
    {
        using namespace player_state_icons_panel_params::defuse_icon_panel_params;

        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(kImageUrl, kTextureHeight);
    
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kAlignment);
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
        uiPanel.setAlign(kAlignment);
        uiPanel.setImageShadow(kShadowParams);
        uiPanel.setWashColor(color);
    }

    void createBlindedIconPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        using namespace player_state_icons_panel_params::blinded_icon_panel_params;

        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(kImageUrl, kTextureHeight);

        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kAlignment);
        uiPanel.setImageShadow(kShadowParams);
    }

    void createBombIconPanel(cs2::CUIPanel* containerPanel) const noexcept
    {
        using namespace player_state_icons_panel_params::bomb_icon_panel_params;

        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(kImageUrl, kTextureHeight);

        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kAlignment);
        uiPanel.setImageShadow(kShadowParams);
        uiPanel.setWashColor(kWashColorCarryingC4);
    }

    void createHealthIconPanel(cs2::CUIPanel* containerPanel) const
    {
        using namespace player_health_panel_params::health_icon_panel_params;

        auto&& healthIconPanel = hookContext.panelFactory().createImagePanel(containerPanel);
        healthIconPanel.setImageSvg(kImageUrl, kTextureHeight);

        auto&& uiPanel = healthIconPanel.uiPanel();
        uiPanel.setAlign(kAlignment);
        uiPanel.setMargin(kMargin);
        uiPanel.setImageShadow(kShadowParams);
    }

    void createHealthTextPanel(cs2::CUIPanel* containerPanel) const
    {
        using namespace player_health_panel_params::health_text_panel_params;

        auto&& label = hookContext.panelFactory().createLabelPanel(containerPanel).uiPanel();
        label.setFont(kFont);
        label.setAlign(kAlignment);
        label.setTextShadow(kShadowParams);
    }

    HookContext& hookContext;
};
