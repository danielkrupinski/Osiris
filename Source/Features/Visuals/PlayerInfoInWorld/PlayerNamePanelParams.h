#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>
#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelFontParams.h>
#include <GameClient/Panorama/PanelMarginParams.h>
#include <GameClient/Panorama/PanelShadowParams.h>

namespace player_name_panel_params::container_panel_params
{
    static constexpr auto kWidth = cs2::CUILength::pixels(256);
    static constexpr auto kHeight = cs2::CUILength::pixels(32);
    static constexpr auto kPositionX = cs2::CUILength::pixels(-kWidth.m_flValue * 0.5f);
    static constexpr auto kPositionY = cs2::CUILength::pixels(-kHeight.m_flValue);
    static constexpr auto kTransformOriginX = cs2::CUILength::percent(50);
    static constexpr auto kTransformOriginY = cs2::CUILength::percent(100);
}

namespace player_name_panel_params::label_params
{
    static constexpr auto kFont = PanelFontParams{
        .fontFamily = "Stratum2, 'Arial Unicode MS'",
        .fontSize = 20,
        .fontWeight = cs2::k_EFontWeightBold
    };
    static constexpr auto kAlignment = PanelAlignmentParams{
        .horizontalAlignment = cs2::k_EHorizontalAlignmentCenter,
        .verticalAlignment = cs2::k_EVerticalAlignmentCenter
    };
    static constexpr auto kColor = cs2::kColorWhite;
    static constexpr auto kShadowParams = PanelShadowParams{
        .horizontalOffset{cs2::CUILength::pixels(0)},
        .verticalOffset{cs2::CUILength::pixels(0)},
        .blurRadius{cs2::CUILength::pixels(3)},
        .strength = 3,
        .color{cs2::kColorBlack}
    };
}
