#pragma once

#include <Features/Visuals/ModelGlow/ModelGlowState.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerStateIcons/PlayerStateIconsToShow.h>
#include <GameClient/Panorama/PanoramaDropDown.h>
#include <GameClient/Panorama/Slider.h>
#include <GameClient/Panorama/TextEntry.h>
#include <Platform/Macros/FunctionAttributes.h>
#include <EntryPoints/GuiEntryPoints.h>

#include "Tabs/VisualsTab/PlayerInfoInWorldDropdownSelectionChangeHandler.h"
#include "Tabs/VisualsTab/PlayerInfoInWorldPlayerHealthColorModeDropdownSelectionChangeHandler.h"
#include "Tabs/VisualsTab/PlayerInfoInWorldPlayerPositionArrowColorModeDropdownSelectionChangeHandler.h"
#include "Tabs/VisualsTab/PlayerModelGlowColorModeDropdownSelectionChangeHandler.h"
#include "Tabs/VisualsTab/PlayerModelGlowDropdownSelectionChangeHandler.h"
#include "Tabs/VisualsTab/PlayerOutlineGlowColorModeDropdownSelectionChangeHandler.h"
#include "Tabs/VisualsTab/PlayerOutlineGlowDropdownSelectionChangeHandler.h"

template <typename HookContext>
class VisualsTab {
public:
    explicit VisualsTab(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void init(auto&& guiPanel) const noexcept
    {
        initPlayerInfoInWorldTab(guiPanel);
        initModelGlowTab(guiPanel);
        initOutlineGlowTab(guiPanel);
        initViewmodelTab(guiPanel);
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        updatePlayerInfoInWorldTab(mainMenu);
        updateOutlineGlowTab(mainMenu);
        updateModelGlowTab(mainMenu);
        updateViewmodelTab(mainMenu);
    }

private:
    void initPlayerInfoInWorldTab(auto&& guiPanel) const
    {
        initDropDown<PlayerInfoInWorldDropdownSelectionChangeHandler<HookContext>>(guiPanel, "player_information_through_walls");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, player_info_vars::PlayerPositionArrowEnabled>>(guiPanel, "player_info_position");
        initDropDown<PlayerInfoInWorldPlayerPositionArrowColorModeDropdownSelectionChangeHandler<HookContext>>(guiPanel, "player_info_position_color");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, player_info_vars::PlayerHealthEnabled>>(guiPanel, "player_info_health");
        initDropDown<PlayerInfoInWorldPlayerHealthColorModeDropdownSelectionChangeHandler<HookContext>>(guiPanel, "player_info_health_color");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, player_info_vars::ActiveWeaponIconEnabled>>(guiPanel, "player_info_weapon");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, player_info_vars::ActiveWeaponAmmoEnabled>>(guiPanel, "player_info_weapon_clip");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, player_info_vars::BombDefuseIconEnabled>>(guiPanel, "player_info_defuse");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, player_info_vars::HostagePickupIconEnabled>>(guiPanel, "player_info_hostage_pickup");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, player_info_vars::HostageRescueIconEnabled>>(guiPanel, "player_info_hostage_rescue");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, player_info_vars::BlindedIconEnabled>>(guiPanel, "player_info_blinded");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, player_info_vars::BombCarrierIconEnabled>>(guiPanel, "player_info_bomb_carrier");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, player_info_vars::BombPlantIconEnabled>>(guiPanel, "player_info_bomb_planting");
    }

    void initModelGlowTab(auto&& guiPanel) const
    {
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, model_glow_vars::Enabled>>(guiPanel, "model_glow_enable");
        initDropDown<PlayerModelGlowDropdownSelectionChangeHandler<HookContext>>(guiPanel, "player_model_glow");
        initDropDown<PlayerModelGlowColorModeDropdownSelectionChangeHandler<HookContext>>(guiPanel, "player_model_glow_color");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, model_glow_vars::GlowWeapons>>(guiPanel, "weapon_model_glow");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, model_glow_vars::GlowDroppedBomb>>(guiPanel, "dropped_bomb_model_glow");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, model_glow_vars::GlowTickingBomb>>(guiPanel, "ticking_bomb_model_glow");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, model_glow_vars::GlowDefuseKits>>(guiPanel, "defuse_kit_model_glow");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, model_glow_vars::GlowGrenadeProjectiles>>(guiPanel, "grenade_proj_model_glow");

        registerHueSliderUpdateHandler<model_glow_vars::PlayerBlueHue, "player_model_glow_blue_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::PlayerGreenHue, "player_model_glow_green_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::PlayerYellowHue, "player_model_glow_yellow_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::PlayerOrangeHue, "player_model_glow_orange_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::PlayerPurpleHue, "player_model_glow_purple_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::TeamTHue, "player_model_glow_t_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::TeamCTHue, "player_model_glow_ct_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::LowHealthHue, "player_model_glow_low_hp_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::HighHealthHue, "player_model_glow_high_hp_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::EnemyHue, "player_model_glow_enemy_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::AllyHue, "player_model_glow_ally_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::MolotovHue, "model_glow_molotov_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::FlashbangHue, "model_glow_flashbang_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::HEGrenadeHue, "model_glow_hegrenade_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::SmokeGrenadeHue, "model_glow_smoke_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::DroppedBombHue, "model_glow_dropped_bomb_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::TickingBombHue, "model_glow_ticking_bomb_hue">(guiPanel);
        registerHueSliderUpdateHandler<model_glow_vars::DefuseKitHue, "model_glow_defuse_kit_hue">(guiPanel);
    }

    void initOutlineGlowTab(auto&& guiPanel) const
    {
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, outline_glow_vars::Enabled>>(guiPanel, "outline_glow_enable");
        initDropDown<PlayerOutlineGlowDropdownSelectionChangeHandler<HookContext>>(guiPanel, "player_outline_glow");
        initDropDown<PlayerOutlineGlowColorModeDropdownSelectionChangeHandler<HookContext>>(guiPanel, "player_outline_glow_color");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, outline_glow_vars::GlowWeapons>>(guiPanel, "weapon_outline_glow");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, outline_glow_vars::GlowDefuseKits>>(guiPanel, "defuse_kit_outline_glow");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, outline_glow_vars::GlowGrenadeProjectiles>>(guiPanel, "grenade_proj_outline_glow");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, outline_glow_vars::GlowDroppedBomb>>(guiPanel, "dropped_bomb_outline_glow");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, outline_glow_vars::GlowTickingBomb>>(guiPanel, "ticking_bomb_outline_glow");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, outline_glow_vars::GlowHostages>>(guiPanel, "hostage_outline_glow");

        registerHueSliderUpdateHandler<outline_glow_vars::PlayerBlueHue, "player_outline_glow_blue_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::PlayerGreenHue, "player_outline_glow_green_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::PlayerYellowHue, "player_outline_glow_yellow_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::PlayerOrangeHue, "player_outline_glow_orange_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::PlayerPurpleHue, "player_outline_glow_purple_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::TeamTHue, "player_outline_glow_t_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::TeamCTHue, "player_outline_glow_ct_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::LowHealthHue, "player_outline_glow_low_hp_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::HighHealthHue, "player_outline_glow_high_hp_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::EnemyHue, "player_outline_glow_enemy_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::AllyHue, "player_outline_glow_ally_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::MolotovHue, "outline_glow_molotov_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::FlashbangHue, "outline_glow_flashbang_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::HEGrenadeHue, "outline_glow_hegrenade_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::SmokeGrenadeHue, "outline_glow_smoke_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::DroppedBombHue, "outline_glow_dropped_bomb_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::TickingBombHue, "outline_glow_ticking_bomb_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::DefuseKitHue, "outline_glow_defuse_kit_hue">(guiPanel);
        registerHueSliderUpdateHandler<outline_glow_vars::HostageHue, "outline_glow_hostage_hue">(guiPanel);
    }

    void initViewmodelTab(auto&& guiPanel) const
    {
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, viewmodel_mod_vars::Enabled>>(guiPanel, "viewmodel_mod");
        initDropDown<OnOffDropdownSelectionChangeHandler<HookContext, viewmodel_mod_vars::ModifyFov>>(guiPanel, "viewmodel_fov_mod");
    }

    template <typename Handler>
    void initDropDown(auto&& guiPanel, const char* panelId) const
    {
        auto&& dropDown = guiPanel.findChildInLayoutFile(panelId).clientPanel().template as<PanoramaDropDown>();
        dropDown.registerSelectionChangedHandler(&GuiEntryPoints<HookContext>::template dropDownSelectionChanged<Handler>);
    }

    template <typename ConfigVariable, TemplateParameterCstring kPanelId>
    void registerHueSliderUpdateHandler(auto&& guiPanel) const
    {
        auto&& hueSlider = hookContext.template make<HueSlider>(guiPanel.findChildInLayoutFile(kPanelId));
        hueSlider.registerSliderValueChangedHandler(&GuiEntryPoints<HookContext>::template hueSliderValueChanged<ConfigVariable, kPanelId>);
        hueSlider.registerTextEntrySubmitHandler(&GuiEntryPoints<HookContext>::template hueSliderTextEntrySubmit<ConfigVariable, kPanelId>);
    }

    void updatePlayerInfoInWorldTab(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "player_information_through_walls", playerInfoDropDownIndex());
        setDropDownSelectedIndex(mainMenu, "player_info_position", !GET_CONFIG_VAR(player_info_vars::PlayerPositionArrowEnabled));
        setDropDownSelectedIndex(mainMenu, "player_info_position_color", static_cast<int>(GET_CONFIG_VAR(player_info_vars::PlayerPositionArrowColorMode)));
        setDropDownSelectedIndex(mainMenu, "player_info_health", !GET_CONFIG_VAR(player_info_vars::PlayerHealthEnabled));
        setDropDownSelectedIndex(mainMenu, "player_info_health_color", static_cast<int>(GET_CONFIG_VAR(player_info_vars::PlayerHealthColorMode)));
        setDropDownSelectedIndex(mainMenu, "player_info_weapon", !GET_CONFIG_VAR(player_info_vars::ActiveWeaponIconEnabled));
        setDropDownSelectedIndex(mainMenu, "player_info_weapon_clip", !GET_CONFIG_VAR(player_info_vars::ActiveWeaponAmmoEnabled));
        setDropDownSelectedIndex(mainMenu, "player_info_bomb_carrier", !GET_CONFIG_VAR(player_info_vars::BombCarrierIconEnabled));
        setDropDownSelectedIndex(mainMenu, "player_info_bomb_planting", !GET_CONFIG_VAR(player_info_vars::BombPlantIconEnabled));
        setDropDownSelectedIndex(mainMenu, "player_info_defuse", !GET_CONFIG_VAR(player_info_vars::BombDefuseIconEnabled));
        setDropDownSelectedIndex(mainMenu, "player_info_hostage_pickup", !GET_CONFIG_VAR(player_info_vars::HostagePickupIconEnabled));
        setDropDownSelectedIndex(mainMenu, "player_info_hostage_rescue", !GET_CONFIG_VAR(player_info_vars::HostageRescueIconEnabled));
        setDropDownSelectedIndex(mainMenu, "player_info_blinded", !GET_CONFIG_VAR(player_info_vars::BlindedIconEnabled));
    }

    void updateOutlineGlowTab(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "outline_glow_enable", !GET_CONFIG_VAR(outline_glow_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "player_outline_glow", playerOutlineGlowDropDownIndex());
        setDropDownSelectedIndex(mainMenu, "player_outline_glow_color", static_cast<int>(GET_CONFIG_VAR(outline_glow_vars::PlayerGlowColorMode)));
        setDropDownSelectedIndex(mainMenu, "weapon_outline_glow", !GET_CONFIG_VAR(outline_glow_vars::GlowWeapons));
        setDropDownSelectedIndex(mainMenu, "grenade_proj_outline_glow", !GET_CONFIG_VAR(outline_glow_vars::GlowWeapons));
        setDropDownSelectedIndex(mainMenu, "dropped_bomb_outline_glow", !GET_CONFIG_VAR(outline_glow_vars::GlowDroppedBomb));
        setDropDownSelectedIndex(mainMenu, "ticking_bomb_outline_glow", !GET_CONFIG_VAR(outline_glow_vars::GlowTickingBomb));
        setDropDownSelectedIndex(mainMenu, "defuse_kit_outline_glow", !GET_CONFIG_VAR(outline_glow_vars::GlowDefuseKits));
        setDropDownSelectedIndex(mainMenu, "hostage_outline_glow", !GET_CONFIG_VAR(outline_glow_vars::GlowHostages));
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
        setDropDownSelectedIndex(mainMenu, "model_glow_enable", !GET_CONFIG_VAR(model_glow_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "player_model_glow", playerModelGlowDropDownIndex());
        setDropDownSelectedIndex(mainMenu, "player_model_glow_color", static_cast<int>(GET_CONFIG_VAR(model_glow_vars::PlayerGlowColorMode)));
        setDropDownSelectedIndex(mainMenu, "weapon_model_glow", !GET_CONFIG_VAR(model_glow_vars::GlowWeapons));
        setDropDownSelectedIndex(mainMenu, "grenade_proj_model_glow", !GET_CONFIG_VAR(model_glow_vars::GlowGrenadeProjectiles));
        setDropDownSelectedIndex(mainMenu, "dropped_bomb_model_glow", !GET_CONFIG_VAR(model_glow_vars::GlowDroppedBomb));
        setDropDownSelectedIndex(mainMenu, "ticking_bomb_model_glow", !GET_CONFIG_VAR(model_glow_vars::GlowTickingBomb));
        setDropDownSelectedIndex(mainMenu, "defuse_kit_model_glow", !GET_CONFIG_VAR(model_glow_vars::GlowDefuseKits));
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
        setDropDownSelectedIndex(mainMenu, "viewmodel_mod", !GET_CONFIG_VAR(viewmodel_mod_vars::Enabled));
        setDropDownSelectedIndex(mainMenu, "viewmodel_fov_mod", !GET_CONFIG_VAR(viewmodel_mod_vars::ModifyFov));
        updateSlider<viewmodel_mod_vars::Fov>(mainMenu, "viewmodel_fov");
    }

    template <typename ConfigVariable>
    void updateSlider(auto&& mainMenu, const char* sliderId) const noexcept
    {
        updateSlider(mainMenu, sliderId, GET_CONFIG_VAR(ConfigVariable));
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
        updateHueSlider(mainMenu, sliderId, GET_CONFIG_VAR(ConfigVariable));
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
        if (GET_CONFIG_VAR(player_info_vars::Enabled))
            return GET_CONFIG_VAR(player_info_vars::OnlyEnemies) ? 0 : 1;
        return 2;
    }

    [[nodiscard]] int playerOutlineGlowDropDownIndex() const noexcept
    {
        if (GET_CONFIG_VAR(outline_glow_vars::GlowPlayers))
            return GET_CONFIG_VAR(outline_glow_vars::GlowOnlyEnemies) ? 0 : 1;
        return 2;
    }

    [[nodiscard]] int playerModelGlowDropDownIndex() const noexcept
    {
        if (GET_CONFIG_VAR(model_glow_vars::GlowPlayers))
            return GET_CONFIG_VAR(model_glow_vars::GlowOnlyEnemies) ? 0 : 1;
        return 2;
    }

    HookContext& hookContext;
};
