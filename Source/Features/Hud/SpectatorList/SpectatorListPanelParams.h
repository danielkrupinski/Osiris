#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>
#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelFontParams.h>
#include <GameClient/Panorama/PanelMarginParams.h>
#include <GameClient/Panorama/PanelShadowParams.h>

namespace spectator_list_panel_params::container_panel_params
{
    constexpr auto kWidth = cs2::CUILength::pixels(240);
    constexpr auto kInitialHeight = cs2::CUILength::pixels(40);
    constexpr auto kPositionX = cs2::CUILength::pixels(-28);
    constexpr auto kPositionY = cs2::CUILength::pixels(-90);
    constexpr auto kAlignment = PanelAlignmentParams{
        .horizontalAlignment = cs2::k_EHorizontalAlignmentRight,
        .verticalAlignment = cs2::k_EVerticalAlignmentCenter};
    constexpr auto kChildrenFlow = cs2::k_EFlowDown;
    constexpr auto kBackgroundColor = cs2::Color{8, 12, 18, 190};
    constexpr auto kBorderColor = cs2::Color{255, 255, 255, 36};
    constexpr auto kBorderWidth = cs2::CUILength::pixels(1);
    constexpr auto kBorderRadius = cs2::CUILength::pixels(5);
    constexpr auto kZIndex = 1000.0f;
    constexpr auto kHeaderHeight = 32.0f;
    constexpr auto kNameRowHeight = 23.0f;
    constexpr auto kBottomPadding = 7.0f;
}

namespace spectator_list_panel_params::header_panel_params
{
    constexpr auto kWidth = cs2::CUILength::percent(100);
    constexpr auto kHeight = cs2::CUILength::pixels(32);
    constexpr auto kFont = PanelFontParams{
        .fontFamily = "Stratum2 Bold TF, 'Arial Unicode MS'",
        .fontSize = 18,
        .fontWeight = cs2::k_EFontWeightBold};
    constexpr auto kColor = cs2::kColorWhite;
    constexpr auto kTextAlign = cs2::k_ETextAlignCenter;
    constexpr auto kShadow = PanelShadowParams{
        .horizontalOffset = cs2::CUILength::pixels(0),
        .verticalOffset = cs2::CUILength::pixels(1),
        .blurRadius = cs2::CUILength::pixels(2),
        .strength = 2.0f,
        .color = cs2::kColorBlack};
}

namespace spectator_list_panel_params::name_panel_params
{
    constexpr auto kWidth = cs2::CUILength::percent(100);
    constexpr auto kHeight = cs2::CUILength::pixels(23);
    constexpr auto kFont = PanelFontParams{
        .fontFamily = "Stratum2, 'Arial Unicode MS'",
        .fontSize = 17};
    constexpr auto kColor = cs2::Color{225, 232, 240};
    constexpr auto kTextAlign = cs2::k_ETextAlignLeft;
    constexpr auto kMargin = PanelMarginParams{
        .marginLeft = cs2::CUILength::pixels(14),
        .marginRight = cs2::CUILength::pixels(10)};
    constexpr auto kShadow = PanelShadowParams{
        .horizontalOffset = cs2::CUILength::pixels(0),
        .verticalOffset = cs2::CUILength::pixels(1),
        .blurRadius = cs2::CUILength::pixels(2),
        .strength = 2.0f,
        .color = cs2::kColorBlack};
}
