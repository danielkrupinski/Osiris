#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>
#include <GameClient/Panorama/PanelShadowParams.h>
#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelMarginParams.h>

namespace player_state_icons_panel_params::container_panel_params
{
    static constexpr auto kAlignment = PanelAlignmentParams{
        .horizontalAlignment = cs2::k_EHorizontalAlignmentCenter,
        .verticalAlignment = cs2::k_EVerticalAlignmentTop
    };
    static constexpr auto kMargin = PanelMarginParams{.marginTop = cs2::CUILength::pixels(3)};
    static constexpr auto kChildrenFlow = cs2::k_EFlowRight;
}

namespace player_state_icons_panel_params::defuse_icon_panel_params
{
    static constexpr auto kImageUrl = "s2r://panorama/images/icons/equipment/defuser.vsvg";
    static constexpr auto kTextureHeight = 24;
    static constexpr auto kAlignment = PanelAlignmentParams{
        .verticalAlignment = cs2::k_EVerticalAlignmentCenter
    };
    static constexpr auto kWashColor = cs2::kColorDefuseKit;
    static constexpr auto kShadowParams = PanelShadowParams{
        .horizontalOffset{cs2::CUILength::pixels(0)},
        .verticalOffset{cs2::CUILength::pixels(0)},
        .blurRadius{cs2::CUILength::pixels(3)},
        .strength = 3,
        .color{cs2::kColorBlack}
    };
}

namespace player_state_icons_panel_params::hostage_icon_panel_params
{
    static constexpr auto kImageUrl = "s2r://panorama/images/icons/ui/hostage_transit.svg";
    static constexpr auto kTextureHeight = 32;
    static constexpr auto kAlignment = PanelAlignmentParams{
        .verticalAlignment = cs2::k_EVerticalAlignmentCenter
    };
    static constexpr auto kShadowParams = PanelShadowParams{
        .horizontalOffset{cs2::CUILength::pixels(0)},
        .verticalOffset{cs2::CUILength::pixels(0)},
        .blurRadius{cs2::CUILength::pixels(3)},
        .strength = 3,
        .color{cs2::kColorBlack}
    };
}

namespace player_state_icons_panel_params::blinded_icon_panel_params
{
    static constexpr auto kImageUrl = "s2r://panorama/images/hud/deathnotice/blind_kill.svg";
    static constexpr auto kTextureHeight = 26;
    static constexpr auto kAlignment = PanelAlignmentParams{
        .verticalAlignment = cs2::k_EVerticalAlignmentCenter
    };
    static constexpr auto kShadowParams = PanelShadowParams{
        .horizontalOffset{cs2::CUILength::pixels(0)},
        .verticalOffset{cs2::CUILength::pixels(0)},
        .blurRadius{cs2::CUILength::pixels(3)},
        .strength = 3,
        .color{cs2::kColorBlack}
    };
}
