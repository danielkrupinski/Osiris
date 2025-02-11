#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>
#include <GameClient/Panorama/PanelShadowParams.h>
#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelFontParams.h>
#include <GameClient/Panorama/PanelMarginParams.h>

namespace player_health_panel_params::container_panel_params
{
    static constexpr auto kAlignment = PanelAlignmentParams{
        .horizontalAlignment = cs2::k_EHorizontalAlignmentCenter,
        .verticalAlignment = cs2::k_EVerticalAlignmentTop
    };
    static constexpr auto kMargin = PanelMarginParams{.marginTop = cs2::CUILength::pixels(1)};
    static constexpr auto kChildrenFlow = cs2::k_EFlowRight;
}

namespace player_health_panel_params::health_icon_panel_params
{
    static constexpr auto kImageUrl = "s2r://panorama/images/hud/health_cross.vsvg";
    static constexpr auto kTextureHeight = 24;
    static constexpr auto kAlignment = PanelAlignmentParams{
        .verticalAlignment = cs2::k_EVerticalAlignmentCenter
    };
    static constexpr auto kMargin = PanelMarginParams{.marginRight = cs2::CUILength::pixels(5)};
    static constexpr auto kShadowParams = PanelShadowParams{
        .horizontalOffset{cs2::CUILength::pixels(0)},
        .verticalOffset{cs2::CUILength::pixels(0)},
        .blurRadius{cs2::CUILength::pixels(3)},
        .strength = 3,
        .color{cs2::kColorBlack}
    };
}

namespace player_health_panel_params::health_text_panel_params
{
    static constexpr auto kFont = PanelFontParams{
        .fontFamily = "Stratum2, 'Arial Unicode MS'",
        .fontSize = 24,
        .fontWeight = cs2::k_EFontWeightBlack
    };
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
