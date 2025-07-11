#pragma once

#include <Features/Visuals/ModelGlow/ModelGlowState.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerStateIcons/PlayerStateIconsToShow.h>
#include <GameClient/Panorama/PanoramaDropDown.h>
#include <GameClient/Panorama/Slider.h>
#include <GameClient/Panorama/TextEntry.h>
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
        setDropDownSelectedIndex(mainMenu, "player_information_through_walls", playerInfoDropDownIndex());
        setDropDownSelectedIndex(mainMenu, "player_info_position", !hookContext.config().template getVariable<PlayerInfoInWorldPlayerPositionArrowEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_position_color", static_cast<int>(hookContext.config().template getVariable<PlayerInfoInWorldPlayerPositionArrowColorMode>()));
        setDropDownSelectedIndex(mainMenu, "player_info_health", !hookContext.config().template getVariable<PlayerInfoInWorldPlayerHealthEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_health_color", static_cast<int>(hookContext.config().template getVariable<PlayerInfoInWorldPlayerHealthColorMode>()));
        setDropDownSelectedIndex(mainMenu, "player_info_weapon", !hookContext.config().template getVariable<PlayerInfoInWorldActiveWeaponIconEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_weapon_clip", !hookContext.config().template getVariable<PlayerInfoInWorldActiveWeaponAmmoEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_bomb_carrier", !hookContext.config().template getVariable<PlayerInfoInWorldBombCarrierIconEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_bomb_planting", !hookContext.config().template getVariable<PlayerInfoInWorldBombPlantIconEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_defuse", !hookContext.config().template getVariable<PlayerInfoInWorldBombDefuseIconEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_hostage_pickup", !hookContext.config().template getVariable<PlayerInfoInWorldHostagePickupIconEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_hostage_rescue", !hookContext.config().template getVariable<PlayerInfoInWorldHostageRescueIconEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_blinded", !hookContext.config().template getVariable<PlayerInfoInWorldBlindedIconEnabled>());
    }

    void updateOutlineGlowTab(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "outline_glow_enable", !hookContext.config().template getVariable<OutlineGlowEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_outline_glow", playerOutlineGlowDropDownIndex());
        setDropDownSelectedIndex(mainMenu, "player_outline_glow_color", static_cast<int>(hookContext.config().template getVariable<PlayerOutlineGlowColorMode>()));
        setDropDownSelectedIndex(mainMenu, "weapon_outline_glow", !hookContext.config().template getVariable<WeaponOutlineGlowEnabled>());
        setDropDownSelectedIndex(mainMenu, "grenade_proj_outline_glow", !hookContext.config().template getVariable<WeaponOutlineGlowEnabled>());
        setDropDownSelectedIndex(mainMenu, "dropped_bomb_outline_glow", !hookContext.config().template getVariable<DroppedBombOutlineGlowEnabled>());
        setDropDownSelectedIndex(mainMenu, "ticking_bomb_outline_glow", !hookContext.config().template getVariable<TickingBombOutlineGlowEnabled>());
        setDropDownSelectedIndex(mainMenu, "defuse_kit_outline_glow", !hookContext.config().template getVariable<DefuseKitOutlineGlowEnabled>());
        setDropDownSelectedIndex(mainMenu, "hostage_outline_glow", !hookContext.config().template getVariable<HostageOutlineGlowEnabled>());
    }

    void updateModelGlowTab(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "model_glow_enable", !hookContext.config().template getVariable<ModelGlowEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_model_glow", playerModelGlowDropDownIndex());
        setDropDownSelectedIndex(mainMenu, "player_model_glow_color", static_cast<int>(hookContext.config().template getVariable<PlayerModelGlowColorMode>()));
        setDropDownSelectedIndex(mainMenu, "weapon_model_glow", !hookContext.config().template getVariable<WeaponModelGlowEnabled>());
        setDropDownSelectedIndex(mainMenu, "grenade_proj_model_glow", !hookContext.config().template getVariable<GrenadeProjectileModelGlowEnabled>());
        setDropDownSelectedIndex(mainMenu, "dropped_bomb_model_glow", !hookContext.config().template getVariable<DroppedBombModelGlowEnabled>());
        setDropDownSelectedIndex(mainMenu, "ticking_bomb_model_glow", !hookContext.config().template getVariable<TickingBombModelGlowEnabled>());
        setDropDownSelectedIndex(mainMenu, "defuse_kit_model_glow", !hookContext.config().template getVariable<DefuseKitModelGlowEnabled>());
        updateHueSlider<PlayerModelGlowPlayerBlueHue>(mainMenu, "player_model_glow_blue_hue");
        updateHueSlider<PlayerModelGlowPlayerGreenHue>(mainMenu, "player_model_glow_green_hue");
        updateHueSlider<PlayerModelGlowPlayerYellowHue>(mainMenu, "player_model_glow_yellow_hue");
        updateHueSlider<PlayerModelGlowPlayerOrangeHue>(mainMenu, "player_model_glow_orange_hue");
        updateHueSlider<PlayerModelGlowPlayerPurpleHue>(mainMenu, "player_model_glow_purple_hue");
        updateHueSlider<PlayerModelGlowTeamTHue>(mainMenu, "player_model_glow_t_hue");
        updateHueSlider<PlayerModelGlowTeamCTHue>(mainMenu, "player_model_glow_ct_hue");
        updateHueSlider<PlayerModelGlowLowHealthHue>(mainMenu, "player_model_glow_low_hp_hue");
        updateHueSlider<PlayerModelGlowHighHealthHue>(mainMenu, "player_model_glow_high_hp_hue");
        updateHueSlider<PlayerModelGlowAllyHue>(mainMenu, "player_model_glow_ally_hue");
        updateHueSlider<PlayerModelGlowEnemyHue>(mainMenu, "player_model_glow_enemy_hue");
        updateHueSlider<ModelGlowMolotovHue>(mainMenu, "model_glow_molotov_hue");
        updateHueSlider<ModelGlowFlashbangHue>(mainMenu, "model_glow_flashbang_hue");
        updateHueSlider<ModelGlowHEGrenadeHue>(mainMenu, "model_glow_hegrenade_hue");
        updateHueSlider<ModelGlowSmokeGrenadeHue>(mainMenu, "model_glow_smoke_hue");
        updateHueSlider<ModelGlowDroppedBombHue>(mainMenu, "model_glow_dropped_bomb_hue");
        updateHueSlider<ModelGlowTickingBombHue>(mainMenu, "model_glow_ticking_bomb_hue");
        updateHueSlider<ModelGlowDefuseKitHue>(mainMenu, "model_glow_defuse_kit_hue");
    }

    template <typename ConfigVariable>
    void updateHueSlider(auto&& mainMenu, const char* sliderId) const noexcept
    {
        updateHueSlider(mainMenu, sliderId, hookContext.config().template getVariable<ConfigVariable>());
    }

    void updateHueSlider(auto&& mainMenu, const char* sliderId, color::HueInteger hue) const noexcept
    {
        auto&& hueSlider = hookContext.template make<HueSlider>(mainMenu.findChildInLayoutFile(sliderId));
        hueSlider.updateSlider(hue);
        hueSlider.updateTextEntry(hue);
        hueSlider.updateColorPreview(hue);
    }

    [[NOINLINE]] void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        mainMenu.findChildInLayoutFile(dropDownId).clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    [[nodiscard]] int playerInfoDropDownIndex() const noexcept
    {
        if (hookContext.config().template getVariable<PlayerInfoInWorldEnabled>())
            return hookContext.config().template getVariable<PlayerInfoInWorldOnlyEnemies>() ? 0 : 1;
        return 2;
    }

    [[nodiscard]] int playerOutlineGlowDropDownIndex() const noexcept
    {
        if (hookContext.config().template getVariable<PlayerOutlineGlowEnabled>())
            return hookContext.config().template getVariable<PlayerOutlineGlowOnlyEnemies>() ? 0 : 1;
        return 2;
    }

    [[nodiscard]] int playerModelGlowDropDownIndex() const noexcept
    {
        if (hookContext.config().template getVariable<PlayerModelGlowEnabled>())
            return hookContext.config().template getVariable<PlayerModelGlowOnlyEnemies>() ? 0 : 1;
        return 2;
    }

    HookContext& hookContext;
};
