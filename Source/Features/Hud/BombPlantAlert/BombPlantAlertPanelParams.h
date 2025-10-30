#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>

#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelFontParams.h>
#include <GameClient/Panorama/PanelMarginParams.h>

namespace bomb_plant_alert_panel_params::container_panel_params
{
    constexpr auto kChildrenFlow = cs2::k_EFlowRight;
    constexpr auto kWidth = cs2::CUILength::percent(100);
    constexpr auto kHeight = cs2::CUILength::pixels(40);
    constexpr auto kPositionX = cs2::CUILength::pixels(0);
    constexpr auto kPositionY = cs2::CUILength::pixels(100);
    constexpr auto kBorderRadius = cs2::CUILength::pixels(5);
}

namespace bomb_plant_alert_panel_params::bombsite_icon_panel_params
{
    constexpr auto kTextureHeight = 32;
    constexpr auto kAlignment = PanelAlignmentParams{.verticalAlignment = cs2::k_EVerticalAlignmentCenter};
    constexpr auto kMargin = PanelMarginParams{.marginLeft = cs2::CUILength::pixels(5)};
}

namespace bomb_plant_alert_panel_params::timer_panel_params
{
    constexpr auto kWidth = cs2::CUILength::fillParentFlow(1.0f);
    constexpr auto kFont = PanelFontParams{
        .fontFamily = "Stratum2 Bold TF, 'Arial Unicode MS'",
        .fontSize = 26};
    constexpr auto kMixBlendMode = cs2::k_EMixBlendModeAdditive;
    constexpr auto kColor = cs2::kColorWhite;
    constexpr auto kAlignment = PanelAlignmentParams{.verticalAlignment = cs2::k_EVerticalAlignmentCenter};
    constexpr auto kTextAlign = cs2::k_ETextAlignCenter;
    constexpr auto kMargin = PanelMarginParams{.marginRight = cs2::CUILength::pixels(5)};
}
