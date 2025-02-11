#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUIPanel.h>
#include <GameClient/Panorama/PanoramaImagePanel.h>
#include <GameClient/Panorama/PanoramaTransformations.h>
#include <Utils/Lvalue.h>

#include "ActiveWeaponAmmo/ActiveWeaponAmmoPanelParams.h"
#include "ActiveWeaponAmmo/PlayerActiveWeaponAmmoPanel.h"
#include "PlayerHealth/PlayerHealthPanel.h"
#include "PlayerHealth/PlayerHealthPanelParams.h"
#include "PlayerInfoContainerPanelParams.h"
#include "PlayerPositionArrow/PlayerPositionArrowPanel.h"
#include "PlayerPositionArrow/PlayerPositionArrowPanelParams.h"
#include "PlayerStateIcons/PlayerStateIconsPanel.h"
#include "PlayerStateIcons/PlayerStateIconsPanelParams.h"
#include "PlayerWeaponIcon/ActiveWeaponIcon/ActiveWeaponIconPanelParams.h"
#include "PlayerWeaponIcon/BombIcon/PlayerBombIconPanelParams.h"
#include "PlayerWeaponIcon/PlayerWeaponIconPanel.h"
#include "PlayerWeaponIcon/PlayerWeaponIconPanelParams.h"

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
    }

    void createPanel(std::type_identity<PlayerWeaponIconPanel<HookContext>>, cs2::CUIPanel* parentPanel) const noexcept
    {
        using namespace player_weapon_icon_panel_params;

        auto&& containerPanel = hookContext.panelFactory().createPanel(parentPanel).uiPanel();
        containerPanel.setFlowChildren(kChildrenFlow);
        containerPanel.setAlign(kAlignment);
        containerPanel.setMargin(kMargin);

        createPlayerActiveWeaponIconPanel(containerPanel);
        createBombIconContainerPanel(containerPanel);
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
        imagePanel.setImageSvg(SvgImageParams{.imageUrl = kImageUrl, .textureHeight = kTextureHeight, .fillColor = kWashColor});
    
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kAlignment);
        uiPanel.setImageShadow(kShadowParams);
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
        imagePanel.setImageSvg(SvgImageParams{.imageUrl = kImageUrl, .textureHeight = kTextureHeight, .fillColor = color});
    
        auto&& uiPanel = imagePanel.uiPanel();
        uiPanel.setAlign(kAlignment);
        uiPanel.setImageShadow(kShadowParams);
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

    void createBombIconContainerPanel(cs2::CUIPanel* parentPanel) const noexcept
    {
        using namespace player_bomb_icon_panel_params::container_panel_params;

        auto&& containerPanel = hookContext.panelFactory().createPanel(parentPanel).uiPanel();
        containerPanel.setAlign(kAlignment);
        containerPanel.setMargin(kMargin);

        createBombIconPanel(containerPanel, kColorCarryingC4);
        createBombIconPanel(containerPanel, kColorPlantingC4);
    }

    void createBombIconPanel(cs2::CUIPanel* containerPanel, cs2::Color color) const noexcept
    {
        using namespace player_bomb_icon_panel_params::bomb_icon_panel_params;

        auto&& imagePanel = hookContext.panelFactory().createImagePanel(containerPanel);
        imagePanel.setImageSvg(SvgImageParams{.imageUrl = kImageUrl, .textureHeight = kTextureHeight, .fillColor = color});
        imagePanel.uiPanel().setImageShadow(kShadowParams);
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

    void createPlayerActiveWeaponIconPanel(cs2::CUIPanel* parentPanel) const noexcept
    {
        using namespace active_weapon_icon_panel_params;

        auto&& panel = hookContext.panelFactory().createImagePanel(parentPanel).uiPanel();
        panel.setAlign(kAlignment);
        panel.setImageShadow(kShadowParams);
    }

    HookContext& hookContext;
};
