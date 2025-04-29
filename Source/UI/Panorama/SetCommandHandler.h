#pragma once

#include <Features/Features.h>
#include <Utils/StringParser.h>

#include <GameClient/Panorama/Slider.h>
#include <GameClient/Panorama/TextEntry.h>
#include "HueSlider.h"

template <typename HookContext>
struct SetCommandHandler {
    SetCommandHandler(StringParser& parser, Features<HookContext> features, HookContext& hookContext) noexcept
        : parser{parser}
        , features{features}
        , hookContext{hookContext}
    {
    }

    void operator()() noexcept
    {
        if (const auto section = parser.getLine('/'); section == "hud") {
            handleHudSection();
        } else if (section == "visuals") {
            handleVisualsSection();
        } else if (section == "sound") {
            handleSoundSection();
        }
    }

private:
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
            handleFeature(features.visualFeatures().playerInfoInWorld());
        } else if (feature == "player_info_position") {
            handleTogglableVariable<PlayerInfoInWorldPlayerPositionArrowEnabled>();
        } else if (feature == "player_info_position_color") {
            features.visualFeatures().playerInfoInWorld().updatePlayerPositionArrowColorMode(parser.getChar());
        } else if (feature == "player_info_health") {
            handleTogglableVariable<PlayerInfoInWorldPlayerHealthEnabled>();
        } else if (feature == "player_info_health_color") {
            features.visualFeatures().playerInfoInWorld().updatePlayerHealthColorMode(parser.getChar());
        } else if (feature == "player_info_weapon") {
            handleTogglableVariable<PlayerInfoInWorldActiveWeaponIconEnabled>();
        } else if (feature == "player_info_weapon_clip") {
            handleTogglableVariable<PlayerInfoInWorldActiveWeaponAmmoEnabled>();
        } else if (feature == "player_info_defuse") {
            handleTogglableVariable<PlayerInfoInWorldBombDefuseIconEnabled>();
        } else if (feature == "player_info_hostage_pickup") {
            handleTogglableVariable<PlayerInfoInWorldHostagePickupIconEnabled>();
        } else if (feature == "player_info_hostage_rescue") {
            handleTogglableVariable<PlayerInfoInWorldHostageRescueIconEnabled>();
        } else if (feature == "player_info_blinded") {
            handleTogglableVariable<PlayerInfoInWorldBlindedIconEnabled>();
        } else if (feature == "player_info_bomb_carrier") {
            handleTogglableVariable<PlayerInfoInWorldBombCarrierIconEnabled>();
        } else if (feature == "player_info_bomb_planting") {
            handleTogglableVariable<PlayerInfoInWorldBombPlantIconEnabled>();
        } else if (feature == "player_outline_glow") {
            handleFeature(features.visualFeatures().playerOutlineGlowToggle());
        } else if (feature == "player_outline_glow_color") {
            features.visualFeatures().playerOutlineGlowToggle().updateColor(parser.getChar());
        } else if (feature == "outline_glow_enable") {
            handleTogglableVariable<OutlineGlowEnabled>();
        } else if (feature == "weapon_outline_glow") {
            handleTogglableVariable<WeaponOutlineGlowEnabled>();
        } else if (feature == "defuse_kit_outline_glow") {
            handleTogglableVariable<DefuseKitOutlineGlowEnabled>();
        } else if (feature == "grenade_proj_outline_glow") {
            handleTogglableVariable<GrenadeProjectileOutlineGlowEnabled>();
        } else if (feature == "dropped_bomb_outline_glow") {
            handleTogglableVariable<DroppedBombOutlineGlowEnabled>();
        } else if (feature == "ticking_bomb_outline_glow") {
            handleTogglableVariable<TickingBombOutlineGlowEnabled>();
        } else if (feature == "hostage_outline_glow") {
            handleTogglableVariable<HostageOutlineGlowEnabled>();
        } else if (feature == "model_glow_enable") {
            features.visualFeatures().modelGlowToggle().updateMasterSwitch(parser.getChar());
        } else if (feature == "player_model_glow") {
            features.visualFeatures().modelGlowToggle().updatePlayerModelGlowToggle(parser.getChar());
        } else if (feature == "player_model_glow_color") {
            features.visualFeatures().modelGlowToggle().updatePlayerModelGlowColor(parser.getChar());
        } else if (feature == "weapon_model_glow") {
            features.visualFeatures().modelGlowToggle().updateWeaponModelGlowToggle(parser.getChar());
        } else if (feature == "dropped_bomb_model_glow") {
            features.visualFeatures().modelGlowToggle().updateDroppedBombModelGlowToggle(parser.getChar());
        } else if (feature == "ticking_bomb_model_glow") {
            features.visualFeatures().modelGlowToggle().updateTickingBombModelGlowToggle(parser.getChar());
        } else if (feature == "defuse_kit_model_glow") {
            features.visualFeatures().modelGlowToggle().updateDefuseKitModelGlowToggle(parser.getChar());
        } else if (feature == "grenade_proj_model_glow") {
            features.visualFeatures().modelGlowToggle().updateGrenadeProjectileModelGlowToggle(parser.getChar());
        } else if (feature == "player_model_glow_blue_hue") {
            handleHueSlider<PlayerModelGlowPlayerBlueHue>("player_model_glow_blue_hue");
        } else if (feature == "player_model_glow_blue_hue_text") {
            handleHueTextEntry<PlayerModelGlowPlayerBlueHue>("player_model_glow_blue_hue");
        } else if (feature == "player_model_glow_green_hue") {
            handleHueSlider<PlayerModelGlowPlayerGreenHue>("player_model_glow_green_hue");
        } else if (feature == "player_model_glow_green_hue_text") {
            handleHueTextEntry<PlayerModelGlowPlayerGreenHue>("player_model_glow_green_hue");
        } else if (feature == "player_model_glow_yellow_hue") {
            handleHueSlider<PlayerModelGlowPlayerYellowHue>("player_model_glow_yellow_hue");
        } else if (feature == "player_model_glow_yellow_hue_text") {
            handleHueTextEntry<PlayerModelGlowPlayerYellowHue>("player_model_glow_yellow_hue");
        } else if (feature == "player_model_glow_orange_hue") {
            handleHueSlider<PlayerModelGlowPlayerOrangeHue>("player_model_glow_orange_hue");
        } else if (feature == "player_model_glow_orange_hue_text") {
            handleHueTextEntry<PlayerModelGlowPlayerOrangeHue>("player_model_glow_orange_hue");
        } else if (feature == "player_model_glow_purple_hue") {
            handleHueSlider<PlayerModelGlowPlayerPurpleHue>("player_model_glow_purple_hue");
        } else if (feature == "player_model_glow_purple_hue_text") {
            handleHueTextEntry<PlayerModelGlowPlayerPurpleHue>("player_model_glow_purple_hue");
        }
    }

    template <typename Feature>
    void handleFeature(Feature&& feature) const noexcept
    {
        feature.update(parser.getChar());
    }

    void handleTogglableFeature(auto&& feature) const noexcept
    {
        switch (parser.getChar()) {
        case '1':
            feature.disable();
            break;
        case '0':
            feature.enable();
            break;
        default:
            break;
        }
    }

    template <typename ConfigVariable>
    void handleTogglableVariable() const noexcept
    {
        switch (parser.getChar()) {
        case '1':
            hookContext.config().template setVariable<ConfigVariable>(false);
            break;
        case '0':
            hookContext.config().template setVariable<ConfigVariable>(true);
            break;
        default:
            break;
        }
    }

    template <typename ConfigVariable>
    void handleHueSlider(const char* sliderId) const noexcept
    {
        const auto hue = parseHueVariable<ConfigVariable>();
        if (!hue.has_value())
            return;

        if (!hookContext.config().template setVariable<ConfigVariable>(*hue))
            return;

        auto&& hueSlider = getHueSlider(sliderId);
        hueSlider.updateTextEntry(*hue);
        hueSlider.updateColorPreview(*hue);
    }

    template <typename ConfigVariable>
    void handleHueTextEntry(const char* sliderId) const noexcept
    {
        auto&& hueSlider = getHueSlider(sliderId);
        const auto hue = parseHueVariable<ConfigVariable>();
        if (!hue.has_value()) {
            hueSlider.updateTextEntry(hookContext.config().template getVariable<ConfigVariable>());
            return;
        }

        if (!hookContext.config().template setVariable<ConfigVariable>(*hue))
            return;

        hueSlider.updateSlider(*hue);
        hueSlider.updateColorPreview(*hue);
    }

    [[nodiscard]] decltype(auto) getHueSlider(const char* sliderId) const noexcept
    {
        const auto mainMenuPointer = hookContext.clientPatternSearchResults().template get<MainMenuPanelPointer>();
        auto&& mainMenu = hookContext.template make<ClientPanel>(mainMenuPointer ? *mainMenuPointer : nullptr).uiPanel();
        return hookContext.template make<HueSlider>(mainMenu.findChildInLayoutFile(sliderId));
    }

    template <typename ConfigVariable>
    [[nodiscard]] std::optional<typename ConfigVariable::ValueType> parseHueVariable() const noexcept
    {
        const auto hueInteger = parseHueInteger();
        if (!hueInteger.has_value())
            return {};

        if (*hueInteger >= ConfigVariable::ValueType::kMin && *hueInteger <= ConfigVariable::ValueType::kMax)
            return typename ConfigVariable::ValueType{*hueInteger};
        return {};
    }

    [[nodiscard]] std::optional<color::HueInteger> parseHueInteger() const noexcept
    {
        if (color::HueInteger::UnderlyingType underlying; parser.parseInt(underlying) && underlying >= color::HueInteger::kMin && underlying <= color::HueInteger::kMax)
            return color::HueInteger{underlying};
        return {};
    }

    StringParser& parser;
    Features<HookContext> features;
    HookContext& hookContext;
};
