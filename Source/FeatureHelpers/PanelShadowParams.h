#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Classes/Panorama.h>

struct PanelShadowParams {
    cs2::CUILength horizontalOffset;
    cs2::CUILength verticalOffset;
    cs2::CUILength blurRadius;
    float strength;
    cs2::Color color;
};
