#pragma once

#include <Features/Features.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <Utils/StringParser.h>

struct SetCommandHandler {
    SetCommandHandler(StringParser& parser, Features features) noexcept
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
            handleTogglableFeature(features.hudFeatures().bombTimer());
        } else if (feature == "defusing_alert") {
            handleTogglableFeature(features.hudFeatures().defusingAlert());
        } else if (feature == "preserve_killfeed") {
            handleTogglableFeature(features.hudFeatures().killfeedPreserver());
        }
    }

    void handleSoundSection() const noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "visualize_player_footsteps") {
            handleTogglableFeature(features.soundFeatures().footstepVisualizer());
        } else if (feature == "visualize_bomb_plant") {
            handleTogglableFeature(features.soundFeatures().bombPlantVisualizer());
        } else if (feature == "visualize_bomb_beep") {
            handleTogglableFeature(features.soundFeatures().bombBeepVisualizer());
        } else if (feature == "visualize_bomb_defuse") {
            handleTogglableFeature(features.soundFeatures().bombDefuseVisualizer());
        } else if (feature == "visualize_scope_sound") {
            handleTogglableFeature(features.soundFeatures().weaponScopeVisualizer());
        } else if (feature == "visualize_reload_sound") {
            handleTogglableFeature(features.soundFeatures().weaponReloadVisualizer());
        }
    }

    void handleVisualsSection() const noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "player_information_through_walls") {
            handleFeature(features.visualFeatures().playerInformationThroughWalls());
        } else if (feature == "player_info_position") {
            handleTogglableFeature(features.visualFeatures().playerPositionToggle());
        } else if (feature == "player_info_health") {
            handleTogglableFeature(features.visualFeatures().playerHealthToggle());
        } else if (feature == "player_info_health_color") {
            handleFeature(features.visualFeatures().playerHealthTextColorToggle());
        } else if (feature == "player_info_weapon") {
            handleTogglableFeature(features.visualFeatures().playerActiveWeaponToggle());
        }
    }

    template <typename Feature>
    void handleFeature(Feature&& feature) const noexcept
    {
        feature.update(parser.getChar());
    }

    template <typename Feature>
    void handleTogglableFeature(TogglableFeature<Feature>&& feature) const noexcept
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
    Features features;
};
