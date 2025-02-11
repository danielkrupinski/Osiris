#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>

#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelFontParams.h>
#include <GameClient/Panorama/PanelMarginParams.h>

namespace bomb_timer_panel_params::container_panel_params
{
    static constexpr auto kChildrenFlow = cs2::k_EFlowRight;
    static constexpr auto kWidth = cs2::CUILength::percent(100);
    static constexpr auto kHeight = cs2::CUILength::pixels(32);
}

namespace bomb_timer_panel_params::bombsite_icon_panel_params
{
    static constexpr auto kWidth = cs2::CUILength::pixels(26);
    static constexpr auto kHeight = cs2::CUILength::pixels(26);
    static constexpr auto kAlignment = PanelAlignmentParams{.verticalAlignment = cs2::k_EVerticalAlignmentCenter};
    static constexpr auto kMargin = PanelMarginParams{.marginLeft = cs2::CUILength::pixels(5)};
}

namespace bomb_timer_panel_params::timer_text_panel_params
{
    static constexpr auto kWidth = cs2::CUILength::fillParentFlow(1.0f);
    static constexpr auto kFont = PanelFontParams{
        .fontFamily = "Stratum2 Bold TF, 'Arial Unicode MS'",
        .fontSize = 22};
    static constexpr auto kMixBlendMode = cs2::k_EMixBlendModeAdditive;
    static constexpr auto kColor = cs2::kColorWhite;
    static constexpr auto kAlignment = PanelAlignmentParams{.verticalAlignment = cs2::k_EVerticalAlignmentCenter};
    static constexpr auto kTextAlign = cs2::k_ETextAlignCenter;
}
