#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>
#include <GameClient/Panorama/PanelShadowParams.h>
#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelMarginParams.h>

namespace player_bomb_icon_panel_params::bomb_icon_panel_params
{
    static constexpr auto kImageUrl = "s2r://panorama/images/icons/equipment/c4.svg";
    static constexpr auto kTextureHeight = 24;
    static constexpr auto kShadowParams = PanelShadowParams{
        .horizontalOffset{cs2::CUILength::pixels(0)},
        .verticalOffset{cs2::CUILength::pixels(0)},
        .blurRadius{cs2::CUILength::pixels(3)},
        .strength = 3,
        .color{cs2::kColorBlack}
    };
}

namespace player_bomb_icon_panel_params::container_panel_params
{
    static constexpr auto kAlignment = PanelAlignmentParams{
        .verticalAlignment = cs2::k_EVerticalAlignmentCenter
    };
    static constexpr auto kMargin = PanelMarginParams{.marginLeft = cs2::CUILength::pixels(2.5), .marginRight = cs2::CUILength::pixels(2.5)};
    static constexpr auto kColorCarryingC4 = cs2::Color{255, 255, 77};
    static constexpr auto kColorPlantingC4 = cs2::Color{255, 193, 77};
}
