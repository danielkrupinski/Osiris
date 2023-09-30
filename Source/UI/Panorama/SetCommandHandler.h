#pragma once

#include <Features/Features.h>
#include <Features/TogglableFeature.h>
#include <Utils/StringParser.h>

struct SetCommandHandler {
    SetCommandHandler(StringParser& parser, Features& features) noexcept
        : parser{ parser }, features{ features }
    {
    }

    void operator()() noexcept
    {
        if (const auto section = parser.getLine('/'); section == "hud") {
            handleHudSection();
        } else if (section == "visuals") {
            handleVisualsSection();
        }
    }

private:
    void handleHudSection() const noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "bomb_timer") {
            handleTogglableFeature(features.hudFeatures.bombTimer);
        } else if (feature == "defusing_alert") {
            handleTogglableFeature(features.hudFeatures.defusingAlert);
        } else if (feature == "preserve_killfeed") {
            handleTogglableFeature(features.hudFeatures.killfeedPreserver);
        }
    }

    void handleVisualsSection() const noexcept
    {
        if (const auto feature = parser.getLine('/'); feature == "remove_scope_overlay") {
            handleTogglableFeature(features.visuals.scopeOverlayRemover);
        }
    }

    template <typename Feature>
    void handleTogglableFeature(TogglableFeature<Feature>& feature) const noexcept
    {
        switch (parser.getChar()) {
        case '0':
            feature.disable();
            break;
        case '1':
            feature.enable();
            break;
        default:
            break;
        }
    }

    StringParser& parser;
    Features& features;
};
