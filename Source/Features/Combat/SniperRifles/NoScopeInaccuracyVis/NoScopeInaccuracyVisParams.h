#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Panorama/CUILength.h>

namespace no_scope_inaccuracy_vis_params
{

constexpr cs2::Color kFallbackColor{255, 255, 255};
constexpr auto kBorderAlpha{255};
constexpr auto kBackgroundAlpha{30};
constexpr auto kBorderWidth{cs2::CUILength::pixels(1)};

}
