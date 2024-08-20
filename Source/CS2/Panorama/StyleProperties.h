#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>
#include <Platform/Macros/PlatformSpecific.h>

namespace cs2
{

struct LINUX_ONLY([[gnu::packed]]) CStyleProperty {
    const void* vmt;
    CStyleSymbol m_symPropertyName;
    bool m_bDisallowTransition;
};

struct CStylePropertyWidth : CStyleProperty {
    static constexpr auto kName{"width"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyWidth@panorama@@", "N8panorama19CStylePropertyWidthE")};

    CUILength m_Length;
};

struct CStylePropertyOpacity : CStyleProperty {
    static constexpr auto kName{"opacity"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyOpacity@panorama@@", "N8panorama21CStylePropertyOpacityE")};

    float opacity;
};

struct CStylePropertyZIndex : CStyleProperty {
    static constexpr auto kName{"z-index"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyZIndex@panorama@@", "N8panorama20CStylePropertyZIndexE")};

    float zindex;
};

struct CStylePropertyHeight : CStyleProperty {
    static constexpr auto kName{"height"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyHeight@panorama@@", "N8panorama20CStylePropertyHeightE")};

    CUILength m_Height;
};

struct CStylePropertyImageShadow : CStyleProperty {
    static constexpr auto kName{"img-shadow"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyImageShadow@panorama@@", "N8panorama25CStylePropertyImageShadowE")};

    bool fullySet;
    CUILength horizontalOffset;
    CUILength verticalOffset;
    CUILength blurRadius;
    float strength;
    Color color;
};

struct CStylePropertyPosition : CStyleProperty {
    static constexpr auto kName{"position"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyPosition@panorama@@", "N8panorama22CStylePropertyPositionE")};

    CUILength x;
    CUILength y;
    CUILength z;
};

struct CStylePropertyTransformOrigin : CStyleProperty {
    static constexpr auto kName{"transform-origin"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyTransformOrigin@panorama@@", "N8panorama29CStylePropertyTransformOriginE")};

    CUILength x;
    CUILength y;
    bool m_bParentRelative;
};

struct CStylePropertyAlign : CStyleProperty {
    static constexpr auto kName{"align"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyAlign@panorama@@", "N8panorama19CStylePropertyAlignE")};

    EHorizontalAlignment m_eHorizontalAlignment;
    EVerticalAlignment m_eVerticalAlignment;
};

struct CStylePropertyWashColor : CStyleProperty {
    static constexpr auto kName{"wash-color"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyWashColor@panorama@@", "N8panorama23CStylePropertyWashColorE")};

    Color color;
    bool fullySet;
};

struct CStylePropertyFlowChildren : CStyleProperty {
    static constexpr auto kName{"flow-children"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyFlowChildren@panorama@@", "N8panorama26CStylePropertyFlowChildrenE")};

    EFlowDirection m_eFlowDirection;
};

struct CStylePropertyFont : CStyleProperty {
    static constexpr auto kName{"font"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyFont@panorama@@", "N8panorama18CStylePropertyFontE")};

    CUtlString m_strFontFamily;
    float m_flFontSize;
    EFontStyle m_eFontStyle;
    EFontWeight m_eFontWeight;
    EFontStretch m_eFontStretch;
};

struct CStylePropertyTextShadow : CStyleProperty {
    static constexpr auto kName{"text-shadow"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyTextShadow@panorama@@", "N8panorama24CStylePropertyTextShadowE")};

    bool fullySet;
    CUILength horizontalOffset;
    CUILength verticalOffset;
    CUILength blurRadius;
    float strength;
    Color color;
};

struct CStylePropertyMixBlendMode : CStyleProperty {
    static constexpr auto kName{"-s2-mix-blend-mode"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyMixBlendMode@panorama@@", "N8panorama26CStylePropertyMixBlendModeE")};

    EMixBlendMode m_eMixBlendMode;
    bool m_bSet;
};

struct CStylePropertyTextAlign : CStyleProperty {
    static constexpr auto kName{"text-align"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyTextAlign@panorama@@", "N8panorama23CStylePropertyTextAlignE")};

    ETextAlign m_eAlign;
};

struct CStylePropertyDimensionsBase : CStyleProperty {
    CUILength m_left;
    CUILength m_top;
    CUILength m_right;
    CUILength m_bottom;
};

struct CStylePropertyMargin : CStylePropertyDimensionsBase {
    static constexpr auto kName{"margin"};
    static constexpr auto kMangledTypeName{WIN64_LINUX(".?AVCStylePropertyMargin@panorama@@", "N8panorama20CStylePropertyMarginE")};
};

};
