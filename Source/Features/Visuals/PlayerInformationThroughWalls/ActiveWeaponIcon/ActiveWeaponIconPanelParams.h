#pragma once

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/ColorConstants.h>
#include <FeatureHelpers/PanelShadowParams.h>

namespace active_weapon_icon_panel_params
{
    static constexpr auto kHorizontalAlignment = cs2::k_EHorizontalAlignmentCenter;
    static constexpr auto kVerticalAlignment = cs2::k_EVerticalAlignmentTop;
    static constexpr auto kMarginLeft = cs2::CUILength::pixels(0);
    static constexpr auto kMarginTop = cs2::CUILength::pixels(3);
    static constexpr auto kMarginRight = cs2::CUILength::pixels(0);
    static constexpr auto kMarginBottom = cs2::CUILength::pixels(0);
    static constexpr auto kShadowParams = PanelShadowParams{
        .horizontalOffset{cs2::CUILength::pixels(0)},
        .verticalOffset{cs2::CUILength::pixels(0)},
        .blurRadius{cs2::CUILength::pixels(3)},
        .strength = 3,
        .color{cs2::kColorBlack}
    };
}
