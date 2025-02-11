#pragma once

#include <CS2/Constants/ColorConstants.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>
#include <GameClient/Panorama/PanelShadowParams.h>
#include <GameClient/Panorama/PanelAlignmentParams.h>
#include <GameClient/Panorama/PanelFontParams.h>
#include <GameClient/Panorama/PanelMarginParams.h>

namespace active_weapon_ammo_panel_params::container_panel_params
{
    static constexpr auto kAlignment = PanelAlignmentParams{
        .horizontalAlignment = cs2::k_EHorizontalAlignmentCenter,
        .verticalAlignment = cs2::k_EVerticalAlignmentTop
    };
    static constexpr auto kMargin = PanelMarginParams{.marginTop = cs2::CUILength::pixels(3)};
    static constexpr auto kChildrenFlow = cs2::k_EFlowRight;
}

namespace active_weapon_ammo_panel_params::ammo_text_panel_params
{
    static constexpr auto kFont = PanelFontParams{
        .fontFamily = "Stratum2, 'Arial Unicode MS'",
        .fontSize = 24,
        .fontWeight = cs2::k_EFontWeightBlack
    };
    static constexpr auto kAlignment = PanelAlignmentParams{
        .verticalAlignment = cs2::k_EVerticalAlignmentCenter
    };
    static constexpr auto kShadowParams = PanelShadowParams{
        .horizontalOffset{cs2::CUILength::pixels(0)},
        .verticalOffset{cs2::CUILength::pixels(0)},
        .blurRadius{cs2::CUILength::pixels(3)},
        .strength = 3,
        .color{cs2::kColorBlack}
    };
    static constexpr auto kColor = cs2::kColorWhite;
}

namespace active_weapon_ammo_panel_params::ammo_icon_panel_params
{
    static constexpr auto kImageUrl = "s2r://panorama/images/hud/bullet_single.vsvg";
    static constexpr auto kTextureHeight = 20;
    static constexpr auto kAlignment = PanelAlignmentParams{
        .verticalAlignment = cs2::k_EVerticalAlignmentCenter
    };
    static constexpr auto kMargin = PanelMarginParams{.marginLeft = cs2::CUILength::pixels(5)};
    static constexpr auto kShadowParams = PanelShadowParams{
        .horizontalOffset{cs2::CUILength::pixels(0)},
        .verticalOffset{cs2::CUILength::pixels(0)},
        .blurRadius{cs2::CUILength::pixels(3)},
        .strength = 3,
        .color{cs2::kColorBlack}
    };
}
