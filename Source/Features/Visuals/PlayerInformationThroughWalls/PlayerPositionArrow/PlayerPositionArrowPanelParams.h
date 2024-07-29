#pragma once

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/ColorConstants.h>
#include <FeatureHelpers/PanelShadowParams.h>

namespace player_position_arrow_panel_params
{
    static constexpr auto kImageUrl = "s2r://panorama/images/hud/reticle/playerid_arrow.vsvg";
    static constexpr auto kTextureHeight = 24;
    static constexpr auto kHorizontalAlignment = cs2::k_EHorizontalAlignmentCenter;
    static constexpr auto kVerticalAlignment = cs2::k_EVerticalAlignmentTop;
    static constexpr auto kShadowParams = PanelShadowParams{
        .horizontalOffset{cs2::CUILength::pixels(0)},
        .verticalOffset{cs2::CUILength::pixels(0)},
        .blurRadius{cs2::CUILength::pixels(3)},
        .strength = 3,
        .color{cs2::kColorBlack}
    };
}
