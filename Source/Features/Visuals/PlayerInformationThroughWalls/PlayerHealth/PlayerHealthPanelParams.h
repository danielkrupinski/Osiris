#pragma once

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/ColorConstants.h>
#include <FeatureHelpers/PanelShadowParams.h>

namespace player_health_panel_params::container_panel_params
{
    static constexpr auto kHorizontalAlignment = cs2::k_EHorizontalAlignmentCenter;
    static constexpr auto kVerticalAlignment = cs2::k_EVerticalAlignmentTop;
    static constexpr auto kMarginLeft = cs2::CUILength::pixels(0);
    static constexpr auto kMarginTop = cs2::CUILength::pixels(1);
    static constexpr auto kMarginRight = cs2::CUILength::pixels(0);
    static constexpr auto kMarginBottom = cs2::CUILength::pixels(0);
    static constexpr auto kChildrenFlow = cs2::k_EFlowRight;
}

namespace player_health_panel_params::health_icon_panel_params
{
    static constexpr auto kImageUrl = "s2r://panorama/images/hud/health_cross.vsvg";
    static constexpr auto kTextureHeight = 24;
    static constexpr auto kHorizontalAlignment = cs2::k_EHorizontalAlignmentUnset;
    static constexpr auto kVerticalAlignment = cs2::k_EVerticalAlignmentCenter;
    static constexpr auto kMarginLeft = cs2::CUILength::pixels(0);
    static constexpr auto kMarginTop = cs2::CUILength::pixels(0);
    static constexpr auto kMarginRight = cs2::CUILength::pixels(5);
    static constexpr auto kMarginBottom = cs2::CUILength::pixels(0);
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
    static constexpr auto kFontFamily = "Stratum2, 'Arial Unicode MS'";
    static constexpr auto kFontSize = 24.0f;
    static constexpr auto kFontWeight = cs2::k_EFontWeightBlack;
    static constexpr auto kHorizontalAlignment = cs2::k_EHorizontalAlignmentUnset;
    static constexpr auto kVerticalAlignment = cs2::k_EVerticalAlignmentCenter;
    static constexpr auto kShadowParams = PanelShadowParams{
        .horizontalOffset{cs2::CUILength::pixels(0)},
        .verticalOffset{cs2::CUILength::pixels(0)},
        .blurRadius{cs2::CUILength::pixels(3)},
        .strength = 3,
        .color{cs2::kColorBlack}
    };
}
