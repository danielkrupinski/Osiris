#pragma once

#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>
#include <GameClient/Panorama/PanelFontParams.h>

namespace post_round_timer_panel_params::container_panel_params
{
    static constexpr auto kWidth = cs2::CUILength::percent(100);
    static constexpr auto kHeight = cs2::CUILength::pixels(32);
}

namespace post_round_timer_panel_params::text_panel_params
{
    static constexpr auto kWidth = cs2::CUILength::percent(100);
    static constexpr auto kMixBlendMode = cs2::k_EMixBlendModeAdditive;
    static constexpr auto kTextAlign = cs2::k_ETextAlignCenter;
    static constexpr auto kFont = PanelFontParams{
        .fontFamily = "Stratum2 Bold TF, 'Arial Unicode MS'",
        .fontSize = 28
    };
}
