#pragma once

#include <CS2/Panorama/StyleProperties.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Utils/TypedStaticStringPool.h>

namespace cs2
{

constexpr auto kStylePropertyTypeNames = TypedStaticStringPool{}
    .add<CStylePropertyWidth>(WIN64_LINUX(".?AVCStylePropertyWidth@panorama@@", "N8panorama19CStylePropertyWidthE"))
    .add<CStylePropertyOpacity>(WIN64_LINUX(".?AVCStylePropertyOpacity@panorama@@", "N8panorama21CStylePropertyOpacityE"))
    .add<CStylePropertyZIndex>(WIN64_LINUX(".?AVCStylePropertyZIndex@panorama@@", "N8panorama20CStylePropertyZIndexE"))
    .add<CStylePropertyHeight>(WIN64_LINUX(".?AVCStylePropertyHeight@panorama@@", "N8panorama20CStylePropertyHeightE"))
    .add<CStylePropertyImageShadow>(WIN64_LINUX(".?AVCStylePropertyImageShadow@panorama@@", "N8panorama25CStylePropertyImageShadowE"))
    .add<CStylePropertyPosition>(WIN64_LINUX(".?AVCStylePropertyPosition@panorama@@", "N8panorama22CStylePropertyPositionE"))
    .add<CStylePropertyTransformOrigin>(WIN64_LINUX(".?AVCStylePropertyTransformOrigin@panorama@@", "N8panorama29CStylePropertyTransformOriginE"))
    .add<CStylePropertyAlign>(WIN64_LINUX(".?AVCStylePropertyAlign@panorama@@", "N8panorama19CStylePropertyAlignE"))
    .add<CStylePropertyWashColor>(WIN64_LINUX(".?AVCStylePropertyWashColor@panorama@@", "N8panorama23CStylePropertyWashColorE"))
    .add<CStylePropertyFlowChildren>(WIN64_LINUX(".?AVCStylePropertyFlowChildren@panorama@@", "N8panorama26CStylePropertyFlowChildrenE"))
    .add<CStylePropertyFont>(WIN64_LINUX(".?AVCStylePropertyFont@panorama@@", "N8panorama18CStylePropertyFontE"))
    .add<CStylePropertyTextShadow>(WIN64_LINUX(".?AVCStylePropertyTextShadow@panorama@@", "N8panorama24CStylePropertyTextShadowE"))
    .add<CStylePropertyMargin>(WIN64_LINUX(".?AVCStylePropertyMargin@panorama@@", "N8panorama20CStylePropertyMarginE"))
    .add<CStylePropertyMixBlendMode>(WIN64_LINUX(".?AVCStylePropertyMixBlendMode@panorama@@", "N8panorama26CStylePropertyMixBlendModeE"))
    .add<CStylePropertyTextAlign>(WIN64_LINUX(".?AVCStylePropertyTextAlign@panorama@@", "N8panorama23CStylePropertyTextAlignE"));

}
