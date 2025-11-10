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
        } else if (feature == "bomb_plant_alert") {
            handleTogglableVariable<BombPlantAlertEnabled>();
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
        const auto newVariableValue = handleIntSlider(sliderId, ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax, GET_CONFIG_VAR(ConfigVariable));
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
        const auto newVariableValue = handleIntSliderTextEntry(sliderId, ConfigVariable::ValueType::kMin, ConfigVariable::ValueType::kMax, GET_CONFIG_VAR(ConfigVariable));
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

    [[nodiscard]] decltype(auto) getIntSlider(const char* sliderId) const noexcept
    {
        const auto mainMenuPointer = hookContext.patternSearchResults().template get<MainMenuPanelPointer>();
        auto&& mainMenu = hookContext.template make<ClientPanel>(mainMenuPointer ? *mainMenuPointer : nullptr).uiPanel();
        return hookContext.template make<IntSlider>(mainMenu.findChildInLayoutFile(sliderId));
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
