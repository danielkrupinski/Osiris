#pragma once

#include <CS2/Classes/Panorama.h>

#include "StylePropertySymbolMap.h"

struct StylePropertiesSymbols {
    explicit StylePropertiesSymbols(StylePropertySymbolMap symbolMap) noexcept
        : width{symbolMap.findSymbol(cs2::CStylePropertyWidth::kName)}
        , opacity{symbolMap.findSymbol(cs2::CStylePropertyOpacity::kName)}
        , zIndex{symbolMap.findSymbol(cs2::CStylePropertyZIndex::kName)}
        , height{symbolMap.findSymbol(cs2::CStylePropertyHeight::kName)}
    {
    }

    cs2::CStyleSymbol width;
    cs2::CStyleSymbol opacity;
    cs2::CStyleSymbol zIndex;
    cs2::CStyleSymbol height;
};
