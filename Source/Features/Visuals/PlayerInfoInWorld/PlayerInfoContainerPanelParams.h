#pragma once

#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>

namespace player_info_container_panel_params
{
    static constexpr auto kWidth = cs2::CUILength::pixels(256);
    static constexpr auto kHeight = cs2::CUILength::pixels(256);
    static constexpr auto kPositionX = cs2::CUILength::pixels(-kWidth.m_flValue * 0.5f);
    static constexpr auto kPositionY = cs2::CUILength::pixels(0.0f);
    static constexpr auto kTransformOriginX = cs2::CUILength::percent(50);
    static constexpr auto kTransformOriginY = cs2::CUILength::percent(0);
    static constexpr auto kChildrenFlow = cs2::k_EFlowDown;
}
