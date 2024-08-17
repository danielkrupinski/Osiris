#pragma once

#include <string_view>

#include <CS2/Classes/Panorama.h>

struct PanelFontParams {
    std::string_view fontFamily;
    float fontSize;
    cs2::EFontWeight fontWeight{cs2::EFontWeight::k_EFontWeightUnset};
};
