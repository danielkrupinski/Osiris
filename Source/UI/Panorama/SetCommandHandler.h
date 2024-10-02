#pragma once

#include <Features/Features.h>
#include <FeatureHelpers/FeatureToggle.h>
#include <Utils/StringParser.h>

template <typename HookContext>
struct SetCommandHandler {
    SetCommandHandler(StringParser& parser, Features<HookContext> features) noexcept
        : parser{parser}
        , features{features}
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
            handleTogglableFeature(features.hudFeatures().bombTimerToggle());
        } else if (feature == "defusing_alert") {
            handleTogglableFeature(features.hudFeatures().defusingAlertToggle());
        } else if (feature == "preserve_killfeed") {
            handleTogglableFeature(features.hudFeatures().killfeedPreserveToggle());
        } else if (feature == "postround_timer") {
            handleTogglableFeature(features.hudFeatures().postRoundTimerToggle());
        }
    }

    void handleSoundSection() const noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "visualize_player_footsteps") {
            handleTogglableFeature(features.soundFeatures().footstepVisualizerToggle());
        } else if (feature == "visualize_bomb_plant") {
            handleTogglableFeature(features.soundFeatures().bombPlantVisualizerToggle());
        } else if (feature == "visualize_bomb_beep") {
            handleTogglableFeature(features.soundFeatures().bombBeepVisualizerToggle());
        } else if (feature == "visualize_bomb_defuse") {
            handleTogglableFeature(features.soundFeatures().bombDefuseVisualizerToggle());
        } else if (feature == "visualize_scope_sound") {
            handleTogglableFeature(features.soundFeatures().weaponScopeVisualizerToggle());
        } else if (feature == "visualize_reload_sound") {
            handleTogglableFeature(features.soundFeatures().weaponReloadVisualizerToggle());
        }
    }

    void handleVisualsSection() const noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "player_information_through_walls") {
            handleFeature(features.visualFeatures().playerInfoInWorld());
        } else if (feature == "player_info_position") {
            handleTogglableFeature(features.visualFeatures().playerPositionToggle());
        } else if (feature == "player_info_position_color") {
            handleFeature(features.visualFeatures().playerPositionArrowColorToggle());
        } else if (feature == "player_info_health") {
            handleTogglableFeature(features.visualFeatures().playerHealthToggle());
        } else if (feature == "player_info_health_color") {
            handleFeature(features.visualFeatures().playerHealthTextColorToggle());
        } else if (feature == "player_info_weapon") {
            handleTogglableFeature(features.visualFeatures().playerActiveWeaponToggle());
        } else if (feature == "player_info_weapon_clip") {
            handleTogglableFeature(features.visualFeatures().playerActiveWeaponAmmoToggle());
        } else if (feature == "player_info_defuse") {
            handleFeature(features.visualFeatures().playerDefuseIconToggle());
        } else if (feature == "player_info_hostage_pickup") {
            handleFeature(features.visualFeatures().hostagePickupIconToggle());
        } else if (feature == "player_info_hostage_rescue") {
            handleFeature(features.visualFeatures().hostageRescueIconToggle());
        } else if (feature == "player_info_blinded") {
            handleFeature(features.visualFeatures().blindedIconToggle());
        } else if (feature == "player_info_bomb_carrier") {
            handleFeature(features.visualFeatures().bombIconToggle());
        } else if (feature == "player_outline_glow") {
            handleFeature(features.visualFeatures().playerOutlineGlowToggle());
        } else if (feature == "player_outline_glow_color") {
            features.visualFeatures().playerOutlineGlowToggle().updateColor(parser.getChar());
        } else if (feature == "outline_glow_enable") {
            handleTogglableFeature(features.visualFeatures().outlineGlowToggle());
        } else if (feature == "weapon_outline_glow") {
            handleTogglableFeature(features.visualFeatures().weaponOutlineGlowToggle());
        } else if (feature == "defuse_kit_outline_glow") {
            handleTogglableFeature(features.visualFeatures().defuseKitOutlineGlowToggle());
        } else if (feature == "grenade_proj_outline_glow") {
            handleTogglableFeature(features.visualFeatures().grenadeProjectileOutlineGlowToggle());
        } else if (feature == "dropped_bomb_outline_glow") {
            handleTogglableFeature(features.visualFeatures().droppedBombOutlineGlowToggle());
        } else if (feature == "ticking_bomb_outline_glow") {
            handleTogglableFeature(features.visualFeatures().tickingBombOutlineGlowToggle());
        } else if (feature == "hostage_outline_glow") {
            handleTogglableFeature(features.visualFeatures().hostageOutlineGlowToggle());
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

    StringParser& parser;
    Features<HookContext> features;
};
