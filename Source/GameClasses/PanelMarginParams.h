#pragma once

#include <CS2/Panorama/CUILength.h>

struct PanelMarginParams {
    cs2::CUILength marginLeft{cs2::CUILength::pixels(0)};
    cs2::CUILength marginTop{cs2::CUILength::pixels(0)};
    cs2::CUILength marginRight{cs2::CUILength::pixels(0)};
    cs2::CUILength marginBottom{cs2::CUILength::pixels(0)};
};
