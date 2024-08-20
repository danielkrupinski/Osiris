#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUILength.h>
#include <FeatureHelpers/PanelShadowParams.h>
#include <GameClasses/PanelAlignmentParams.h>
#include <GameClasses/PanelMarginParams.h>

namespace active_weapon_icon_panel_params
{
    static constexpr auto kAlignment = PanelAlignmentParams{
        .horizontalAlignment = cs2::k_EHorizontalAlignmentCenter,
        .verticalAlignment = cs2::k_EVerticalAlignmentTop
    };
    static constexpr auto kMargin = PanelMarginParams{.marginTop = cs2::CUILength::pixels(3)};
    static constexpr auto kShadowParams = PanelShadowParams{
        .horizontalOffset{cs2::CUILength::pixels(0)},
        .verticalOffset{cs2::CUILength::pixels(0)},
        .blurRadius{cs2::CUILength::pixels(3)},
        .strength = 3,
        .color{cs2::kColorBlack}
    };
}
