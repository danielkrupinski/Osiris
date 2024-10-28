#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Classes/CUtlString.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>
#include <Platform/Macros/PlatformSpecific.h>

#include "CStyleSymbol.h"

namespace cs2
{

struct LINUX_ONLY([[gnu::packed]]) CStyleProperty {
    const void* vmt;
    CStyleSymbol m_symPropertyName;
    bool m_bDisallowTransition;
};

struct CStylePropertyWidth : CStyleProperty {
    CUILength m_Length;
};

struct CStylePropertyOpacity : CStyleProperty {
    float opacity;
};

struct CStylePropertyZIndex : CStyleProperty {
    float zindex;
};

struct CStylePropertyHeight : CStyleProperty {
    CUILength m_Height;
};

struct CStylePropertyImageShadow : CStyleProperty {
    bool fullySet;
    CUILength horizontalOffset;
    CUILength verticalOffset;
    CUILength blurRadius;
    float strength;
    Color color;
};

struct CStylePropertyPosition : CStyleProperty {
    CUILength x;
    CUILength y;
    CUILength z;
};

struct CStylePropertyTransformOrigin : CStyleProperty {
    CUILength x;
    CUILength y;
    bool m_bParentRelative;
};

struct CStylePropertyAlign : CStyleProperty {
    EHorizontalAlignment m_eHorizontalAlignment;
    EVerticalAlignment m_eVerticalAlignment;
};

struct CStylePropertyWashColor : CStyleProperty {
    Color color;
    bool fullySet;
};

struct CStylePropertyFlowChildren : CStyleProperty {
    EFlowDirection m_eFlowDirection;
};

struct CStylePropertyFont : CStyleProperty {
    CUtlString m_strFontFamily;
    float m_flFontSize;
    EFontStyle m_eFontStyle;
    EFontWeight m_eFontWeight;
    EFontStretch m_eFontStretch;
};

struct CStylePropertyTextShadow : CStyleProperty {
    bool fullySet;
    CUILength horizontalOffset;
    CUILength verticalOffset;
    CUILength blurRadius;
    float strength;
    Color color;
};

struct CStylePropertyMixBlendMode : CStyleProperty {
    EMixBlendMode m_eMixBlendMode;
    bool m_bSet;
};

struct CStylePropertyTextAlign : CStyleProperty {
    ETextAlign m_eAlign;
};

struct CStylePropertyDimensionsBase : CStyleProperty {
    CUILength m_left;
    CUILength m_top;
    CUILength m_right;
    CUILength m_bottom;
};

struct CStylePropertyMargin : CStylePropertyDimensionsBase {
};

};
