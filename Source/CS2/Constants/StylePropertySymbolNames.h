#pragma once

#include <CS2/Panorama/StyleProperties.h>
#include <Utils/TypedStaticStringPool.h>

namespace cs2
{
    
constexpr auto kStylePropertySymbolNames = TypedStaticStringPool{}
    .add<CStylePropertyWidth>("width")
    .add<CStylePropertyOpacity>("opacity")
    .add<CStylePropertyZIndex>("z-index")
    .add<CStylePropertyHeight>("height")
    .add<CStylePropertyImageShadow>("img-shadow")
    .add<CStylePropertyPosition>("position")
    .add<CStylePropertyTransformOrigin>("transform-origin")
    .add<CStylePropertyAlign>("align")
    .add<CStylePropertyWashColor>("wash-color")
    .add<CStylePropertyFlowChildren>("flow-children")
    .add<CStylePropertyFont>("font")
    .add<CStylePropertyTextShadow>("text-shadow")
    .add<CStylePropertyMargin>("margin")
    .add<CStylePropertyMixBlendMode>("-s2-mix-blend-mode")
    .add<CStylePropertyTextAlign>("text-align");

}
