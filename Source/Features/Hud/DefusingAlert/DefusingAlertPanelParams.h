#pragma once

#include <CS2/Panorama/CUILength.h>
#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/StyleEnums.h>
#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelFontParams.h>
#include <GameClient/Panorama/PanelMarginParams.h>

namespace defusing_alert_panel_params::timer_text_panel_params
{
    constexpr auto kWidth = cs2::CUILength::fillParentFlow(1.0f);
    constexpr auto kFont = PanelFontParams{
        .fontFamily = "Stratum2 Bold TF, 'Arial Unicode MS'",
        .fontSize = 24};
    constexpr auto kMixBlendMode = cs2::k_EMixBlendModeAdditive;
    constexpr auto kColor = cs2::kColorWhite;
    constexpr auto kAlignment = PanelAlignmentParams{.verticalAlignment = cs2::k_EVerticalAlignmentCenter};
    constexpr auto kTextAlign = cs2::k_ETextAlignCenter;
}

namespace defusing_alert_panel_params::defuse_icon_panel_params
{
    constexpr auto kImageUrl = "s2r://panorama/images/icons/equipment/defuser.vsvg";
    constexpr auto kTextureHeight = 25;
    constexpr auto kAlignment = PanelAlignmentParams{.verticalAlignment = cs2::k_EVerticalAlignmentCenter};
    constexpr auto kColor = cs2::kColorDefuseKit;
    constexpr auto kMargin = PanelMarginParams{.marginLeft = cs2::CUILength::pixels(5)};
}
