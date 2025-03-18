#pragma once

#include <Features/Features.h>
#include <Utils/StringParser.h>

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

    StringParser& parser;
    Features<HookContext> features;
    HookContext& hookContext;
};
