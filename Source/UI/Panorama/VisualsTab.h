#pragma once

#include <Features/Visuals/ModelGlow/ModelGlowState.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerStateIcons/PlayerStateIconsToShow.h>
#include <GameClient/Panorama/PanoramaDropDown.h>
#include <Platform/Macros/FunctionAttributes.h>

template <typename HookContext>
class VisualsTab {
public:
    explicit VisualsTab(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        updatePlayerInfoInWorldTab(mainMenu);
        updateOutlineGlowTab(mainMenu);
        updateModelGlowTab(mainMenu);
    }

private:
    void updatePlayerInfoInWorldTab(auto&& mainMenu) const noexcept
    {
        const auto& playerInfoInWorldState = hookContext.featuresStates().visualFeaturesStates.playerInfoInWorldState;
        setDropDownSelectedIndex(mainMenu, "player_information_through_walls", playerInfoDropDownIndex(playerInfoInWorldState));
        setDropDownSelectedIndex(mainMenu, "player_info_position", !playerInfoInWorldState.showPlayerPosition);
        setDropDownSelectedIndex(mainMenu, "player_info_position_color", static_cast<int>(playerInfoInWorldState.playerPositionArrowColor));
        setDropDownSelectedIndex(mainMenu, "player_info_health", !playerInfoInWorldState.showPlayerHealth);
        setDropDownSelectedIndex(mainMenu, "player_info_health_color", static_cast<int>(playerInfoInWorldState.playerHealthTextColor));
        setDropDownSelectedIndex(mainMenu, "player_info_weapon", !playerInfoInWorldState.showPlayerActiveWeapon);
        setDropDownSelectedIndex(mainMenu, "player_info_weapon_clip", !playerInfoInWorldState.showPlayerActiveWeaponAmmo);
        setDropDownSelectedIndex(mainMenu, "player_info_bomb_carrier", !playerInfoInWorldState.showBombCarrierIcon);
        setDropDownSelectedIndex(mainMenu, "player_info_bomb_planting", !playerInfoInWorldState.showBombPlantingIcon);
        setDropDownSelectedIndex(mainMenu, "player_info_defuse", !playerInfoInWorldState.playerStateIconsToShow.template has<DefuseIconPanel>());
        setDropDownSelectedIndex(mainMenu, "player_info_hostage_pickup", !playerInfoInWorldState.playerStateIconsToShow.template has<HostagePickupPanel>());
        setDropDownSelectedIndex(mainMenu, "player_info_hostage_rescue", !playerInfoInWorldState.playerStateIconsToShow.template has<HostageRescuePanel>());
        setDropDownSelectedIndex(mainMenu, "player_info_blinded", !playerInfoInWorldState.playerStateIconsToShow.template has<BlindedIconPanel>());
    }

    void updateOutlineGlowTab(auto&& mainMenu) const noexcept
    {
        const auto& outlineGlowState = hookContext.featuresStates().visualFeaturesStates.outlineGlowState;
        setDropDownSelectedIndex(mainMenu, "outline_glow_enable", !outlineGlowState.masterSwitch);
        setDropDownSelectedIndex(mainMenu, "player_outline_glow", playerOutlineGlowDropDownIndex(outlineGlowState));
        setDropDownSelectedIndex(mainMenu, "player_outline_glow_color", static_cast<int>(outlineGlowState.playerGlowColorType));
        setDropDownSelectedIndex(mainMenu, "weapon_outline_glow", !outlineGlowState.enabledForWeapons);
        setDropDownSelectedIndex(mainMenu, "grenade_proj_outline_glow", !outlineGlowState.enabledForGrenadeProjectiles);
        setDropDownSelectedIndex(mainMenu, "dropped_bomb_outline_glow", !outlineGlowState.enabledForDroppedBomb);
        setDropDownSelectedIndex(mainMenu, "ticking_bomb_outline_glow", !outlineGlowState.enabledForTickingBomb);
        setDropDownSelectedIndex(mainMenu, "defuse_kit_outline_glow", !outlineGlowState.enabledForDefuseKits);
        setDropDownSelectedIndex(mainMenu, "hostage_outline_glow", !outlineGlowState.enabledForHostages);
    }

    void updateModelGlowTab(auto&& mainMenu) const noexcept
    {
        const auto& modelGlowState = hookContext.featuresStates().visualFeaturesStates.modelGlowState;
        setDropDownSelectedIndex(mainMenu, "model_glow_enable", modelGlowState.masterSwitch != ModelGlowState::State::Enabled);
        setDropDownSelectedIndex(mainMenu, "player_model_glow", playerModelGlowDropDownIndex(modelGlowState));
        setDropDownSelectedIndex(mainMenu, "player_model_glow_color", static_cast<int>(modelGlowState.playerModelGlowColorType));
        setDropDownSelectedIndex(mainMenu, "weapon_model_glow", modelGlowState.weaponModelGlow != ModelGlowState::State::Enabled);
        setDropDownSelectedIndex(mainMenu, "grenade_proj_model_glow", modelGlowState.grenadeProjectileModelGlow != ModelGlowState::State::Enabled);
        setDropDownSelectedIndex(mainMenu, "dropped_bomb_model_glow", modelGlowState.droppedBombModelGlow != ModelGlowState::State::Enabled);
        setDropDownSelectedIndex(mainMenu, "ticking_bomb_model_glow", modelGlowState.tickingBombModelGlow != ModelGlowState::State::Enabled);
        setDropDownSelectedIndex(mainMenu, "defuse_kit_model_glow", modelGlowState.defuseKitModelGlow != ModelGlowState::State::Enabled);
    }

    [[NOINLINE]] void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        mainMenu.findChildInLayoutFile(dropDownId).clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    [[nodiscard]] int playerInfoDropDownIndex(const auto& playerInfoInWorldState) const noexcept
    {
        if (playerInfoInWorldState.enabled)
            return playerInfoInWorldState.showOnlyEnemies ? 0 : 1;
        return 2;
    }

    [[nodiscard]] int playerOutlineGlowDropDownIndex(const auto& outlineGlowState) const noexcept
    {
        if (outlineGlowState.enabledForPlayers)
            return outlineGlowState.showOnlyEnemies ? 0 : 1;
        return 2;
    }

    [[nodiscard]] int playerModelGlowDropDownIndex(const auto& modelGlowState) const noexcept
    {
        if (modelGlowState.playerModelGlow == ModelGlowState::State::Enabled)
            return modelGlowState.showOnlyEnemies ? 0 : 1;
        return 2;
    }

    HookContext& hookContext;
};
