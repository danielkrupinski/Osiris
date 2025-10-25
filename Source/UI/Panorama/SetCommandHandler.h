#pragma once

#include <Features/Combat/SniperRifles/NoScopeInaccuracyVis/NoScopeInaccuracyVisConfigVariables.h>
#include <Features/Visuals/ModelGlow/ModelGlowToggle.h>
#include <Features/Visuals/OutlineGlow/PlayerOutlineGlow/PlayerOutlineGlowToggle.h>
#include <Features/Visuals/PlayerInfoInWorld/PlayerInfoInWorld.h>
#include <GameClient/Panorama/Slider.h>
#include <GameClient/Panorama/TextEntry.h>
#include <Platform/Macros/FunctionAttributes.h>
#include <Utils/StringParser.h>
#include "Tabs/VisualsTab/HueSlider.h"
#include "Tabs/VisualsTab/IntSlider.h"

template <typename HookContext>
struct SetCommandHandler {
    SetCommandHandler(StringParser& parser, HookContext& hookContext) noexcept
        : parser{parser}
        , hookContext{hookContext}
    {
    }

    void operator()() noexcept
    {
        if (const auto section = parser.getLine('/'); section == "combat") {
            handleCombatSection();
        } else if (section == "hud") {
            handleHudSection();
        } else if (section == "visuals") {
            handleVisualsSection();
        } else if (section == "sound") {
            handleSoundSection();
        }
    }

private:
    void handleCombatSection() noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "no_scope_inacc_vis") {
            handleTogglableVariable<no_scope_inaccuracy_vis_vars::Enabled>();
        }
    }

    void handleHudSection() const noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "bomb_timer") {
            handleTogglableVariable<BombTimerEnabled>();
        } else if (feature == "defusing_alert") {
            handleTogglableVariable<DefusingAlertEnabled>();
        } else if (feature == "preserve_killfeed") {
            handleTogglableVariable<KillfeedPreserverEnabled>();
        } else if (feature == "postround_timer") {
            handleTogglableVariable<PostRoundTimerEnabled>();
        }
    }

    void handleSoundSection() const noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "visualize_player_footsteps") {
            handleTogglableVariable<FootstepSoundVisualizationEnabled>();
        } else if (feature == "visualize_bomb_plant") {
            handleTogglableVariable<BombPlantSoundVisualizationEnabled>();
        } else if (feature == "visualize_bomb_beep") {
            handleTogglableVariable<BombBeepSoundVisualizationEnabled>();
        } else if (feature == "visualize_bomb_defuse") {
            handleTogglableVariable<BombDefuseSoundVisualizationEnabled>();
        } else if (feature == "visualize_scope_sound") {
            handleTogglableVariable<WeaponScopeSoundVisualizationEnabled>();
        } else if (feature == "visualize_reload_sound") {
            handleTogglableVariable<WeaponReloadSoundVisualizationEnabled>();
        }
    }

    void handleVisualsSection() const noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "player_information_through_walls") {
            handleFeature(playerInfoInWorldToggle());
        } else if (feature == "player_info_position") {
            handleTogglableVariable<player_info_vars::PlayerPositionArrowEnabled>();
        } else if (feature == "player_info_position_color") {
            playerInfoInWorldToggle().updatePlayerPositionArrowColorMode(parser.getChar());
        } else if (feature == "player_info_health") {
            handleTogglableVariable<player_info_vars::PlayerHealthEnabled>();
        } else if (feature == "player_info_health_color") {
            playerInfoInWorldToggle().updatePlayerHealthColorMode(parser.getChar());
        } else if (feature == "player_info_weapon") {
            handleTogglableVariable<player_info_vars::ActiveWeaponIconEnabled>();
        } else if (feature == "player_info_weapon_clip") {
            handleTogglableVariable<player_info_vars::ActiveWeaponAmmoEnabled>();
        } else if (feature == "player_info_defuse") {
            handleTogglableVariable<player_info_vars::BombDefuseIconEnabled>();
        } else if (feature == "player_info_hostage_pickup") {
            handleTogglableVariable<player_info_vars::HostagePickupIconEnabled>();
        } else if (feature == "player_info_hostage_rescue") {
            handleTogglableVariable<player_info_vars::HostageRescueIconEnabled>();
        } else if (feature == "player_info_blinded") {
            handleTogglableVariable<player_info_vars::BlindedIconEnabled>();
        } else if (feature == "player_info_bomb_carrier") {
            handleTogglableVariable<player_info_vars::BombCarrierIconEnabled>();
        } else if (feature == "player_info_bomb_planting") {
            handleTogglableVariable<player_info_vars::BombPlantIconEnabled>();
        } else if (feature == "player_outline_glow") {
            handleFeature(playerOutlineGlowToggle());
        } else if (feature == "player_outline_glow_color") {
            playerOutlineGlowToggle().updateColor(parser.getChar());
        } else if (feature == "outline_glow_enable") {
            handleTogglableVariable<outline_glow_vars::Enabled>();
        } else if (feature == "weapon_outline_glow") {
            handleTogglableVariable<outline_glow_vars::GlowWeapons>();
        } else if (feature == "defuse_kit_outline_glow") {
            handleTogglableVariable<outline_glow_vars::GlowDefuseKits>();
        } else if (feature == "grenade_proj_outline_glow") {
            handleTogglableVariable<outline_glow_vars::GlowGrenadeProjectiles>();
        } else if (feature == "dropped_bomb_outline_glow") {
            handleTogglableVariable<outline_glow_vars::GlowDroppedBomb>();
        } else if (feature == "ticking_bomb_outline_glow") {
            handleTogglableVariable<outline_glow_vars::GlowTickingBomb>();
        } else if (feature == "hostage_outline_glow") {
            handleTogglableVariable<outline_glow_vars::GlowHostages>();
        } else if (feature == "model_glow_enable") {
            modelGlowToggle().updateMasterSwitch(parser.getChar());
        } else if (feature == "player_model_glow") {
            modelGlowToggle().updatePlayerModelGlowToggle(parser.getChar());
        } else if (feature == "player_model_glow_color") {
            modelGlowToggle().updatePlayerModelGlowColor(parser.getChar());
        } else if (feature == "weapon_model_glow") {
            modelGlowToggle().updateWeaponModelGlowToggle(parser.getChar());
        } else if (feature == "dropped_bomb_model_glow") {
            modelGlowToggle().updateDroppedBombModelGlowToggle(parser.getChar());
        } else if (feature == "ticking_bomb_model_glow") {
            modelGlowToggle().updateTickingBombModelGlowToggle(parser.getChar());
        } else if (feature == "defuse_kit_model_glow") {
            modelGlowToggle().updateDefuseKitModelGlowToggle(parser.getChar());
        } else if (feature == "grenade_proj_model_glow") {
            modelGlowToggle().updateGrenadeProjectileModelGlowToggle(parser.getChar());
        } else if (feature == "player_model_glow_blue_hue") {
            handleHueSlider<model_glow_vars::PlayerBlueHue>("player_model_glow_blue_hue");
        } else if (feature == "player_model_glow_blue_hue_text") {
            handleHueTextEntry<model_glow_vars::PlayerBlueHue>("player_model_glow_blue_hue");
        } else if (feature == "player_model_glow_green_hue") {
            handleHueSlider<model_glow_vars::PlayerGreenHue>("player_model_glow_green_hue");
        } else if (feature == "player_model_glow_green_hue_text") {
            handleHueTextEntry<model_glow_vars::PlayerGreenHue>("player_model_glow_green_hue");
        } else if (feature == "player_model_glow_yellow_hue") {
            handleHueSlider<model_glow_vars::PlayerYellowHue>("player_model_glow_yellow_hue");
        } else if (feature == "player_model_glow_yellow_hue_text") {
            handleHueTextEntry<model_glow_vars::PlayerYellowHue>("player_model_glow_yellow_hue");
        } else if (feature == "player_model_glow_orange_hue") {
            handleHueSlider<model_glow_vars::PlayerOrangeHue>("player_model_glow_orange_hue");
        } else if (feature == "player_model_glow_orange_hue_text") {
            handleHueTextEntry<model_glow_vars::PlayerOrangeHue>("player_model_glow_orange_hue");
        } else if (feature == "player_model_glow_purple_hue") {
            handleHueSlider<model_glow_vars::PlayerPurpleHue>("player_model_glow_purple_hue");
        } else if (feature == "player_model_glow_purple_hue_text") {
            handleHueTextEntry<model_glow_vars::PlayerPurpleHue>("player_model_glow_purple_hue");
        } else if (feature == "player_model_glow_t_hue") {
            handleHueSlider<model_glow_vars::TeamTHue>("player_model_glow_t_hue");
        } else if (feature == "player_model_glow_t_hue_text") {
            handleHueTextEntry<model_glow_vars::TeamTHue>("player_model_glow_t_hue");
        } else if (feature == "player_model_glow_ct_hue") {
            handleHueSlider<model_glow_vars::TeamCTHue>("player_model_glow_ct_hue");
        } else if (feature == "player_model_glow_ct_hue_text") {
            handleHueTextEntry<model_glow_vars::TeamCTHue>("player_model_glow_ct_hue");
        } else if (feature == "player_model_glow_low_hp_hue") {
            handleHueSlider<model_glow_vars::LowHealthHue>("player_model_glow_low_hp_hue");
        } else if (feature == "player_model_glow_low_hp_hue_text") {
            handleHueTextEntry<model_glow_vars::LowHealthHue>("player_model_glow_low_hp_hue");
        } else if (feature == "player_model_glow_high_hp_hue") {
            handleHueSlider<model_glow_vars::HighHealthHue>("player_model_glow_high_hp_hue");
        } else if (feature == "player_model_glow_high_hp_hue_text") {
            handleHueTextEntry<model_glow_vars::HighHealthHue>("player_model_glow_high_hp_hue");
        } else if (feature == "player_model_glow_enemy_hue") {
            handleHueSlider<model_glow_vars::EnemyHue>("player_model_glow_enemy_hue");
        } else if (feature == "player_model_glow_enemy_hue_text") {
            handleHueTextEntry<model_glow_vars::EnemyHue>("player_model_glow_enemy_hue");
        } else if (feature == "player_model_glow_ally_hue") {
            handleHueSlider<model_glow_vars::AllyHue>("player_model_glow_ally_hue");
        } else if (feature == "player_model_glow_ally_hue_text") {
            handleHueTextEntry<model_glow_vars::AllyHue>("player_model_glow_ally_hue");
        } else if (feature == "model_glow_molotov_hue") {
            handleHueSlider<model_glow_vars::MolotovHue>("model_glow_molotov_hue");
        } else if (feature == "model_glow_molotov_hue_text") {
            handleHueTextEntry<model_glow_vars::MolotovHue>("model_glow_molotov_hue");
        } else if (feature == "model_glow_flashbang_hue") {
            handleHueSlider<model_glow_vars::FlashbangHue>("model_glow_flashbang_hue");
        } else if (feature == "model_glow_flashbang_hue_text") {
            handleHueTextEntry<model_glow_vars::FlashbangHue>("model_glow_flashbang_hue");
        } else if (feature == "model_glow_hegrenade_hue") {
            handleHueSlider<model_glow_vars::HEGrenadeHue>("model_glow_hegrenade_hue");
        } else if (feature == "model_glow_hegrenade_hue_text") {
            handleHueTextEntry<model_glow_vars::HEGrenadeHue>("model_glow_hegrenade_hue");
        } else if (feature == "model_glow_smoke_hue") {
            handleHueSlider<model_glow_vars::SmokeGrenadeHue>("model_glow_smoke_hue");
        } else if (feature == "model_glow_smoke_hue_text") {
            handleHueTextEntry<model_glow_vars::SmokeGrenadeHue>("model_glow_smoke_hue");
        } else if (feature == "model_glow_dropped_bomb_hue") {
            handleHueSlider<model_glow_vars::DroppedBombHue>("model_glow_dropped_bomb_hue");
        } else if (feature == "model_glow_dropped_bomb_hue_text") {
            handleHueTextEntry<model_glow_vars::DroppedBombHue>("model_glow_dropped_bomb_hue");
        } else if (feature == "model_glow_ticking_bomb_hue") {
            handleHueSlider<model_glow_vars::TickingBombHue>("model_glow_ticking_bomb_hue");
        } else if (feature == "model_glow_ticking_bomb_hue_text") {
            handleHueTextEntry<model_glow_vars::TickingBombHue>("model_glow_ticking_bomb_hue");
        } else if (feature == "model_glow_defuse_kit_hue") {
            handleHueSlider<model_glow_vars::DefuseKitHue>("model_glow_defuse_kit_hue");
        } else if (feature == "model_glow_defuse_kit_hue_text") {
            handleHueTextEntry<model_glow_vars::DefuseKitHue>("model_glow_defuse_kit_hue");
        } else if (feature == "player_outline_glow_blue_hue") {
            handleHueSlider<outline_glow_vars::PlayerBlueHue>("player_outline_glow_blue_hue");
        } else if (feature == "player_outline_glow_blue_hue_text") {
            handleHueTextEntry<outline_glow_vars::PlayerBlueHue>("player_outline_glow_blue_hue");
        } else if (feature == "player_outline_glow_green_hue") {
            handleHueSlider<outline_glow_vars::PlayerGreenHue>("player_outline_glow_green_hue");
        } else if (feature == "player_outline_glow_green_hue_text") {
            handleHueTextEntry<outline_glow_vars::PlayerGreenHue>("player_outline_glow_green_hue");
        } else if (feature == "player_outline_glow_yellow_hue") {
            handleHueSlider<outline_glow_vars::PlayerYellowHue>("player_outline_glow_yellow_hue");
        } else if (feature == "player_outline_glow_yellow_hue_text") {
            handleHueTextEntry<outline_glow_vars::PlayerYellowHue>("player_outline_glow_yellow_hue");
        } else if (feature == "player_outline_glow_orange_hue") {
            handleHueSlider<outline_glow_vars::PlayerOrangeHue>("player_outline_glow_orange_hue");
        } else if (feature == "player_outline_glow_orange_hue_text") {
            handleHueTextEntry<outline_glow_vars::PlayerOrangeHue>("player_outline_glow_orange_hue");
        } else if (feature == "player_outline_glow_purple_hue") {
            handleHueSlider<outline_glow_vars::PlayerPurpleHue>("player_outline_glow_purple_hue");
        } else if (feature == "player_outline_glow_purple_hue_text") {
            handleHueTextEntry<outline_glow_vars::PlayerPurpleHue>("player_outline_glow_purple_hue");
        } else if (feature == "player_outline_glow_t_hue") {
            handleHueSlider<outline_glow_vars::TeamTHue>("player_outline_glow_t_hue");
        } else if (feature == "player_outline_glow_t_hue_text") {
            handleHueTextEntry<outline_glow_vars::TeamTHue>("player_model_glow_t_hue");
        } else if (feature == "player_outline_glow_ct_hue") {
            handleHueSlider<outline_glow_vars::TeamCTHue>("player_outline_glow_ct_hue");
        } else if (feature == "player_outline_glow_ct_hue_text") {
            handleHueTextEntry<outline_glow_vars::TeamCTHue>("player_outline_glow_ct_hue");
        } else if (feature == "player_outline_glow_low_hp_hue") {
            handleHueSlider<outline_glow_vars::LowHealthHue>("player_outline_glow_low_hp_hue");
        } else if (feature == "player_outline_glow_low_hp_hue_text") {
            handleHueTextEntry<outline_glow_vars::LowHealthHue>("player_outline_glow_low_hp_hue");
        } else if (feature == "player_outline_glow_high_hp_hue") {
            handleHueSlider<outline_glow_vars::HighHealthHue>("player_outline_glow_high_hp_hue");
        } else if (feature == "player_outline_glow_high_hp_hue_text") {
            handleHueTextEntry<outline_glow_vars::HighHealthHue>("player_outline_glow_high_hp_hue");
        } else if (feature == "player_outline_glow_enemy_hue") {
            handleHueSlider<outline_glow_vars::EnemyHue>("player_outline_glow_enemy_hue");
        } else if (feature == "player_outline_glow_enemy_hue_text") {
            handleHueTextEntry<outline_glow_vars::EnemyHue>("player_outline_glow_enemy_hue");
        } else if (feature == "player_outline_glow_ally_hue") {
            handleHueSlider<outline_glow_vars::AllyHue>("player_outline_glow_ally_hue");
        } else if (feature == "player_outline_glow_ally_hue_text") {
            handleHueTextEntry<outline_glow_vars::AllyHue>("player_outline_glow_ally_hue");
        } else if (feature == "outline_glow_molotov_hue") {
            handleHueSlider<outline_glow_vars::MolotovHue>("outline_glow_molotov_hue");
        } else if (feature == "outline_glow_molotov_hue_text") {
            handleHueTextEntry<outline_glow_vars::MolotovHue>("outline_glow_molotov_hue");
        } else if (feature == "outline_glow_flashbang_hue") {
            handleHueSlider<outline_glow_vars::FlashbangHue>("outline_glow_flashbang_hue");
        } else if (feature == "outline_glow_flashbang_hue_text") {
            handleHueTextEntry<outline_glow_vars::FlashbangHue>("outline_glow_flashbang_hue");
        } else if (feature == "outline_glow_hegrenade_hue") {
            handleHueSlider<outline_glow_vars::HEGrenadeHue>("outline_glow_hegrenade_hue");
        } else if (feature == "outline_glow_hegrenade_hue_text") {
            handleHueTextEntry<outline_glow_vars::HEGrenadeHue>("outline_glow_hegrenade_hue");
        } else if (feature == "outline_glow_smoke_hue") {
            handleHueSlider<outline_glow_vars::SmokeGrenadeHue>("outline_glow_smoke_hue");
        } else if (feature == "outline_glow_smoke_hue_text") {
            handleHueTextEntry<outline_glow_vars::SmokeGrenadeHue>("outline_glow_smoke_hue");
        } else if (feature == "outline_glow_dropped_bomb_hue") {
            handleHueSlider<outline_glow_vars::DroppedBombHue>("outline_glow_dropped_bomb_hue");
        } else if (feature == "outline_glow_dropped_bomb_hue_text") {
            handleHueTextEntry<outline_glow_vars::DroppedBombHue>("outline_glow_dropped_bomb_hue");
        } else if (feature == "outline_glow_ticking_bomb_hue") {
            handleHueSlider<outline_glow_vars::TickingBombHue>("outline_glow_ticking_bomb_hue");
        } else if (feature == "outline_glow_ticking_bomb_hue_text") {
            handleHueTextEntry<outline_glow_vars::TickingBombHue>("outline_glow_ticking_bomb_hue");
        } else if (feature == "outline_glow_defuse_kit_hue") {
            handleHueSlider<outline_glow_vars::DefuseKitHue>("outline_glow_defuse_kit_hue");
        } else if (feature == "outline_glow_defuse_kit_hue_text") {
            handleHueTextEntry<outline_glow_vars::DefuseKitHue>("outline_glow_defuse_kit_hue");
        } else if (feature == "outline_glow_hostage_hue") {
            handleHueSlider<outline_glow_vars::HostageHue>("outline_glow_hostage_hue");
        } else if (feature == "outline_glow_hostage_hue_text") {
            handleHueTextEntry<outline_glow_vars::HostageHue>("outline_glow_hostage_hue");
        } else if (feature == "viewmodel_mod") {
            handleTogglableVariable<viewmodel_mod_vars::Enabled>();
        } else if (feature == "viewmodel_fov_mod") {
            handleTogglableVariable<viewmodel_mod_vars::ModifyFov>();  
        } else if (feature == "viewmodel_fov") {
            handleIntSlider<viewmodel_mod_vars::Fov>("viewmodel_fov");
        } else if (feature == "viewmodel_fov_text") {
            handleIntSliderTextEntry<viewmodel_mod_vars::Fov>("viewmodel_fov");
        }
    }

    template <typename Feature>
    void handleFeature(Feature&& feature) const noexcept
    {
        feature.update(parser.getChar());
    }

    template <typename ConfigVariable>
    void handleTogglableVariable() const noexcept
    {
        if (const auto option = parser.getChar(); option == '0' || option == '1')
            hookContext.config().template setVariable<ConfigVariable>(option == '0');
    }

    template <typename ConfigVariable>
    void handleIntSlider(const char* sliderId) const noexcept
    {
        const auto newVariableValue = handleIntSlider(sliderId, ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax, hookContext.config().template getVariable<ConfigVariable>());
        hookContext.config().template setVariable<ConfigVariable>(typename ConfigVariable::ValueType{newVariableValue});
    }

    [[nodiscard]] std::uint8_t handleIntSlider(const char* sliderId, std::uint8_t min, std::uint8_t max, std::uint8_t current) const noexcept
    {
        std::uint8_t value{};
        if (!parser.parseInt(value) || value == current || value < min || value > max)
            return current;

        auto&& hueSlider = getIntSlider(sliderId);
        hueSlider.updateTextEntry(value);
        return value;
    }

    template <typename ConfigVariable>
    void handleIntSliderTextEntry(const char* sliderId) const noexcept
    {
        const auto newVariableValue = handleIntSliderTextEntry(sliderId, ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax, hookContext.config().template getVariable<ConfigVariable>());
        hookContext.config().template setVariable<ConfigVariable>(typename ConfigVariable::ValueType{newVariableValue});
    }

    [[nodiscard]] std::uint8_t handleIntSliderTextEntry(const char* sliderId, std::uint8_t min, std::uint8_t max, std::uint8_t current) const noexcept
    {
        auto&& slider = getIntSlider(sliderId);
        std::uint8_t value{};
        if (!parser.parseInt(value) || value < min || value > max) {
            slider.updateTextEntry(current);
            return current;
        }

        if (value == current)
            return current;

        slider.updateSlider(value);
        return value;
    }

    template <typename ConfigVariable>
    void handleHueSlider(const char* sliderId) const noexcept
    {
        const auto newVariableValue = handleHueSlider(sliderId, ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax, hookContext.config().template getVariable<ConfigVariable>());
        hookContext.config().template setVariable<ConfigVariable>(typename ConfigVariable::ValueType{newVariableValue});
    }

    [[nodiscard]] color::HueInteger handleHueSlider(const char* sliderId, color::HueInteger min, color::HueInteger max, color::HueInteger current) const noexcept
    {
        const auto hue = parseHueVariable(min, max);
        if (!hue.has_value() || *hue == current)
            return current;

        auto&& hueSlider = getHueSlider(sliderId);
        hueSlider.updateTextEntry(*hue);
        hueSlider.updateColorPreview(*hue);
        return *hue;
    }

    template <typename ConfigVariable>
    void handleHueTextEntry(const char* sliderId) const noexcept
    {
        const auto newVariableValue = handleHueTextEntry(sliderId, ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax, hookContext.config().template getVariable<ConfigVariable>());
        hookContext.config().template setVariable<ConfigVariable>(typename ConfigVariable::ValueType{newVariableValue});
    }

    [[nodiscard]] color::HueInteger handleHueTextEntry(const char* sliderId, color::HueInteger min, color::HueInteger max, color::HueInteger current) const noexcept
    {
        auto&& hueSlider = getHueSlider(sliderId);
        const auto hue = parseHueVariable(min, max);
        if (!hue.has_value()) {
            hueSlider.updateTextEntry(current);
            return current;
        }

        if (*hue == current)
            return current;

        hueSlider.updateSlider(*hue);
        hueSlider.updateColorPreview(*hue);
        return *hue;
    }

    [[nodiscard]] decltype(auto) getHueSlider(const char* sliderId) const noexcept
    {
        const auto mainMenuPointer = hookContext.patternSearchResults().template get<MainMenuPanelPointer>();
        auto&& mainMenu = hookContext.template make<ClientPanel>(mainMenuPointer ? *mainMenuPointer : nullptr).uiPanel();
        return hookContext.template make<HueSlider>(mainMenu.findChildInLayoutFile(sliderId));
    }

    [[nodiscard]] decltype(auto) getIntSlider(const char* sliderId) const noexcept
    {
        const auto mainMenuPointer = hookContext.patternSearchResults().template get<MainMenuPanelPointer>();
        auto&& mainMenu = hookContext.template make<ClientPanel>(mainMenuPointer ? *mainMenuPointer : nullptr).uiPanel();
        return hookContext.template make<IntSlider>(mainMenu.findChildInLayoutFile(sliderId));
    }

    [[nodiscard]] [[NOINLINE]] std::optional<color::HueInteger> parseHueVariable(color::HueInteger min, color::HueInteger max) const noexcept
    {
        const auto hueInteger = parseHueInteger();
        if (!hueInteger.has_value())
            return {};

        if (*hueInteger >= min && *hueInteger <= max)
            return *hueInteger;
        return {};
    }

    [[nodiscard]] std::optional<color::HueInteger> parseHueInteger() const noexcept
    {
        if (color::HueInteger::UnderlyingType underlying; parser.parseInt(underlying) && underlying >= color::HueInteger::kMin && underlying <= color::HueInteger::kMax)
            return color::HueInteger{underlying};
        return {};
    }

    [[nodiscard]] auto playerInfoInWorldToggle() const noexcept
    {
        return PlayerInfoInWorldToggle{hookContext};
    }

    [[nodiscard]] decltype(auto) playerOutlineGlowToggle() const noexcept
    {
        return hookContext.template make<PlayerOutlineGlowToggle>();
    }

    [[nodiscard]] decltype(auto) modelGlowToggle() const noexcept
    {
        return hookContext.template make<ModelGlowToggle>();
    }

    StringParser& parser;
    HookContext& hookContext;
};
