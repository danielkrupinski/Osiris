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
        updateViewmodelTab(mainMenu);
    }

private:
    void updatePlayerInfoInWorldTab(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "player_information_through_walls", playerInfoDropDownIndex());
        setDropDownSelectedIndex(mainMenu, "player_info_position", !hookContext.config().template getVariable<player_info_vars::PlayerPositionArrowEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_position_color", static_cast<int>(hookContext.config().template getVariable<player_info_vars::PlayerPositionArrowColorMode>()));
        setDropDownSelectedIndex(mainMenu, "player_info_health", !hookContext.config().template getVariable<player_info_vars::PlayerHealthEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_health_color", static_cast<int>(hookContext.config().template getVariable<player_info_vars::PlayerHealthColorMode>()));
        setDropDownSelectedIndex(mainMenu, "player_info_weapon", !hookContext.config().template getVariable<player_info_vars::ActiveWeaponIconEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_weapon_clip", !hookContext.config().template getVariable<player_info_vars::ActiveWeaponAmmoEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_bomb_carrier", !hookContext.config().template getVariable<player_info_vars::BombCarrierIconEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_bomb_planting", !hookContext.config().template getVariable<player_info_vars::BombPlantIconEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_defuse", !hookContext.config().template getVariable<player_info_vars::BombDefuseIconEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_hostage_pickup", !hookContext.config().template getVariable<player_info_vars::HostagePickupIconEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_hostage_rescue", !hookContext.config().template getVariable<player_info_vars::HostageRescueIconEnabled>());
        setDropDownSelectedIndex(mainMenu, "player_info_blinded", !hookContext.config().template getVariable<player_info_vars::BlindedIconEnabled>());
    }

    void updateOutlineGlowTab(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "outline_glow_enable", !hookContext.config().template getVariable<outline_glow_vars::Enabled>());
        setDropDownSelectedIndex(mainMenu, "player_outline_glow", playerOutlineGlowDropDownIndex());
        setDropDownSelectedIndex(mainMenu, "player_outline_glow_color", static_cast<int>(hookContext.config().template getVariable<outline_glow_vars::PlayerGlowColorMode>()));
        setDropDownSelectedIndex(mainMenu, "weapon_outline_glow", !hookContext.config().template getVariable<outline_glow_vars::GlowWeapons>());
        setDropDownSelectedIndex(mainMenu, "grenade_proj_outline_glow", !hookContext.config().template getVariable<outline_glow_vars::GlowWeapons>());
        setDropDownSelectedIndex(mainMenu, "dropped_bomb_outline_glow", !hookContext.config().template getVariable<outline_glow_vars::GlowDroppedBomb>());
        setDropDownSelectedIndex(mainMenu, "ticking_bomb_outline_glow", !hookContext.config().template getVariable<outline_glow_vars::GlowTickingBomb>());
        setDropDownSelectedIndex(mainMenu, "defuse_kit_outline_glow", !hookContext.config().template getVariable<outline_glow_vars::GlowDefuseKits>());
        setDropDownSelectedIndex(mainMenu, "hostage_outline_glow", !hookContext.config().template getVariable<outline_glow_vars::GlowHostages>());
        updateHueSlider<outline_glow_vars::PlayerBlueHue>(mainMenu, "player_outline_glow_blue_hue");
        updateHueSlider<outline_glow_vars::PlayerGreenHue>(mainMenu, "player_outline_glow_green_hue");
        updateHueSlider<outline_glow_vars::PlayerYellowHue>(mainMenu, "player_outline_glow_yellow_hue");
        updateHueSlider<outline_glow_vars::PlayerOrangeHue>(mainMenu, "player_outline_glow_orange_hue");
        updateHueSlider<outline_glow_vars::PlayerPurpleHue>(mainMenu, "player_outline_glow_purple_hue");
        updateHueSlider<outline_glow_vars::TeamTHue>(mainMenu, "player_outline_glow_t_hue");
        updateHueSlider<outline_glow_vars::TeamCTHue>(mainMenu, "player_outline_glow_ct_hue");
        updateHueSlider<outline_glow_vars::LowHealthHue>(mainMenu, "player_outline_glow_low_hp_hue");
        updateHueSlider<outline_glow_vars::HighHealthHue>(mainMenu, "player_outline_glow_high_hp_hue");
        updateHueSlider<outline_glow_vars::AllyHue>(mainMenu, "player_outline_glow_ally_hue");
        updateHueSlider<outline_glow_vars::EnemyHue>(mainMenu, "player_outline_glow_enemy_hue");
        updateHueSlider<outline_glow_vars::MolotovHue>(mainMenu, "outline_glow_molotov_hue");
        updateHueSlider<outline_glow_vars::FlashbangHue>(mainMenu, "outline_glow_flashbang_hue");
        updateHueSlider<outline_glow_vars::HEGrenadeHue>(mainMenu, "outline_glow_hegrenade_hue");
        updateHueSlider<outline_glow_vars::SmokeGrenadeHue>(mainMenu, "outline_glow_smoke_hue");
        updateHueSlider<outline_glow_vars::DroppedBombHue>(mainMenu, "outline_glow_dropped_bomb_hue");
        updateHueSlider<outline_glow_vars::TickingBombHue>(mainMenu, "outline_glow_ticking_bomb_hue");
        updateHueSlider<outline_glow_vars::DefuseKitHue>(mainMenu, "outline_glow_defuse_kit_hue");
        updateHueSlider<outline_glow_vars::HostageHue>(mainMenu, "outline_glow_hostage_hue");
    }

    void updateModelGlowTab(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "model_glow_enable", !hookContext.config().template getVariable<model_glow_vars::Enabled>());
        setDropDownSelectedIndex(mainMenu, "player_model_glow", playerModelGlowDropDownIndex());
        setDropDownSelectedIndex(mainMenu, "player_model_glow_color", static_cast<int>(hookContext.config().template getVariable<model_glow_vars::PlayerGlowColorMode>()));
        setDropDownSelectedIndex(mainMenu, "weapon_model_glow", !hookContext.config().template getVariable<model_glow_vars::GlowWeapons>());
        setDropDownSelectedIndex(mainMenu, "grenade_proj_model_glow", !hookContext.config().template getVariable<model_glow_vars::GlowGrenadeProjectiles>());
        setDropDownSelectedIndex(mainMenu, "dropped_bomb_model_glow", !hookContext.config().template getVariable<model_glow_vars::GlowDroppedBomb>());
        setDropDownSelectedIndex(mainMenu, "ticking_bomb_model_glow", !hookContext.config().template getVariable<model_glow_vars::GlowTickingBomb>());
        setDropDownSelectedIndex(mainMenu, "defuse_kit_model_glow", !hookContext.config().template getVariable<model_glow_vars::GlowDefuseKits>());
        updateHueSlider<model_glow_vars::PlayerBlueHue>(mainMenu, "player_model_glow_blue_hue");
        updateHueSlider<model_glow_vars::PlayerGreenHue>(mainMenu, "player_model_glow_green_hue");
        updateHueSlider<model_glow_vars::PlayerYellowHue>(mainMenu, "player_model_glow_yellow_hue");
        updateHueSlider<model_glow_vars::PlayerOrangeHue>(mainMenu, "player_model_glow_orange_hue");
        updateHueSlider<model_glow_vars::PlayerPurpleHue>(mainMenu, "player_model_glow_purple_hue");
        updateHueSlider<model_glow_vars::TeamTHue>(mainMenu, "player_model_glow_t_hue");
        updateHueSlider<model_glow_vars::TeamCTHue>(mainMenu, "player_model_glow_ct_hue");
        updateHueSlider<model_glow_vars::LowHealthHue>(mainMenu, "player_model_glow_low_hp_hue");
        updateHueSlider<model_glow_vars::HighHealthHue>(mainMenu, "player_model_glow_high_hp_hue");
        updateHueSlider<model_glow_vars::AllyHue>(mainMenu, "player_model_glow_ally_hue");
        updateHueSlider<model_glow_vars::EnemyHue>(mainMenu, "player_model_glow_enemy_hue");
        updateHueSlider<model_glow_vars::MolotovHue>(mainMenu, "model_glow_molotov_hue");
        updateHueSlider<model_glow_vars::FlashbangHue>(mainMenu, "model_glow_flashbang_hue");
        updateHueSlider<model_glow_vars::HEGrenadeHue>(mainMenu, "model_glow_hegrenade_hue");
        updateHueSlider<model_glow_vars::SmokeGrenadeHue>(mainMenu, "model_glow_smoke_hue");
        updateHueSlider<model_glow_vars::DroppedBombHue>(mainMenu, "model_glow_dropped_bomb_hue");
        updateHueSlider<model_glow_vars::TickingBombHue>(mainMenu, "model_glow_ticking_bomb_hue");
        updateHueSlider<model_glow_vars::DefuseKitHue>(mainMenu, "model_glow_defuse_kit_hue");
    }

    void updateViewmodelTab(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "viewmodel_mod", !hookContext.config().template getVariable<viewmodel_mod_vars::Enabled>());
        setDropDownSelectedIndex(mainMenu, "viewmodel_fov_mod", !hookContext.config().template getVariable<viewmodel_mod_vars::ModifyFov>());
        updateSlider<viewmodel_mod_vars::Fov>(mainMenu, "viewmodel_fov");
    }

    template <typename ConfigVariable>
    void updateSlider(auto&& mainMenu, const char* sliderId) const noexcept
    {
        updateSlider(mainMenu, sliderId, hookContext.config().template getVariable<ConfigVariable>());
    }

    void updateSlider(auto&& mainMenu, const char* sliderId, std::uint8_t value) const noexcept
    {
        auto&& slider = hookContext.template make<IntSlider>(mainMenu.findChildInLayoutFile(sliderId));
        slider.updateSlider(value);
        slider.updateTextEntry(value);
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
        if (hookContext.config().template getVariable<player_info_vars::Enabled>())
            return hookContext.config().template getVariable<player_info_vars::OnlyEnemies>() ? 0 : 1;
        return 2;
    }

    [[nodiscard]] int playerOutlineGlowDropDownIndex() const noexcept
    {
        if (hookContext.config().template getVariable<outline_glow_vars::GlowPlayers>())
            return hookContext.config().template getVariable<outline_glow_vars::GlowOnlyEnemies>() ? 0 : 1;
        return 2;
    }

    [[nodiscard]] int playerModelGlowDropDownIndex() const noexcept
    {
        if (hookContext.config().template getVariable<model_glow_vars::GlowPlayers>())
            return hookContext.config().template getVariable<model_glow_vars::GlowOnlyEnemies>() ? 0 : 1;
        return 2;
    }

    HookContext& hookContext;
};
