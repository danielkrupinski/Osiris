#pragma once

#include <CS2/Classes/Panorama.h>

#include "StylePropertySymbolMap.h"

struct StylePropertiesSymbols {
    explicit StylePropertiesSymbols(StylePropertySymbolMap symbolMap) noexcept
        : widthSymbol{symbolMap.findSymbol(cs2::CStylePropertyWidth::kName)}
        , opacitySymbol{symbolMap.findSymbol(cs2::CStylePropertyOpacity::kName)}
        , zIndexSymbol{symbolMap.findSymbol(cs2::CStylePropertyZIndex::kName)}
        , heightSymbol{symbolMap.findSymbol(cs2::CStylePropertyHeight::kName)}
        , imageShadowSymbol{symbolMap.findSymbol(cs2::CStylePropertyImageShadow::kName)}
        , positionSymbol{symbolMap.findSymbol(cs2::CStylePropertyPosition::kName)}
        , transformOriginSymbol{symbolMap.findSymbol(cs2::CStylePropertyTransformOrigin::kName)}
        , alignSymbol{symbolMap.findSymbol(cs2::CStylePropertyAlign::kName)}
    {
    }

    cs2::CStyleSymbol widthSymbol;
    cs2::CStyleSymbol opacitySymbol;
    cs2::CStyleSymbol zIndexSymbol;
    cs2::CStyleSymbol heightSymbol;
    cs2::CStyleSymbol imageShadowSymbol;
    cs2::CStyleSymbol positionSymbol;
    cs2::CStyleSymbol transformOriginSymbol;
    cs2::CStyleSymbol alignSymbol;
};
