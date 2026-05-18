#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>
#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelFontParams.h>
#include <GameClient/Panorama/PanelMarginParams.h>
#include <GameClient/Panorama/PanelShadowParams.h>

namespace player_name_panel_params
{
    static constexpr auto kFont = PanelFontParams{
        .fontFamily = "Stratum2, 'Arial Unicode MS'",
        .fontSize = 20,
        .fontWeight = cs2::k_EFontWeightBold
    };
    static constexpr auto kAlignment = PanelAlignmentParams{
        .horizontalAlignment = cs2::k_EHorizontalAlignmentCenter,
        .verticalAlignment = cs2::k_EVerticalAlignmentTop
    };
    static constexpr auto kMargin = PanelMarginParams{.marginBottom = cs2::CUILength::pixels(1)};
    static constexpr auto kColor = cs2::kColorWhite;
    static constexpr auto kShadowParams = PanelShadowParams{
        .horizontalOffset{cs2::CUILength::pixels(0)},
        .verticalOffset{cs2::CUILength::pixels(0)},
        .blurRadius{cs2::CUILength::pixels(3)},
        .strength = 3,
        .color{cs2::kColorBlack}
    };
}
