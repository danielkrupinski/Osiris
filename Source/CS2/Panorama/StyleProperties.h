#pragma once

#include <CS2/Classes/Color.h>
#include <CS2/Classes/CUtlString.h>
#include <CS2/Classes/VMatrix.h>
#include <CS2/Panorama/CUILength.h>
#include <CS2/Panorama/StyleEnums.h>
#include <Platform/Macros/PlatformSpecific.h>

#include "CStyleSymbol.h"
#include "Transform3D.h"

namespace cs2
{

struct CStyleProperty {
};

#define BASE_STYLE_PROPERTY_FIELDS() \
const void* vmt; \
CStyleSymbol m_symPropertyName; \
bool m_bDisallowTransition; \
WIN64_ONLY(std::byte pad[6]{};)

struct CStylePropertyWidth : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    CUILength m_Length;
};
static_assert(std::is_standard_layout_v<CStylePropertyWidth>);
static_assert(sizeof(CStylePropertyWidth) == 24);
static_assert(offsetof(CStylePropertyWidth, m_Length) == WIN64_LINUX(16, 12));

struct CStylePropertyOpacity : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    float opacity;
};
static_assert(std::is_standard_layout_v<CStylePropertyOpacity>);
static_assert(sizeof(CStylePropertyOpacity) == WIN64_LINUX(24, 16));
static_assert(offsetof(CStylePropertyOpacity, opacity) == WIN64_LINUX(16, 12));

struct CStylePropertyZIndex : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    float zindex;
};
static_assert(std::is_standard_layout_v<CStylePropertyZIndex>);
static_assert(sizeof(CStylePropertyZIndex) == WIN64_LINUX(24, 16));
static_assert(offsetof(CStylePropertyZIndex, zindex) == WIN64_LINUX(16, 12));

struct CStylePropertyHeight : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    CUILength m_Height;
};
static_assert(std::is_standard_layout_v<CStylePropertyHeight>);
static_assert(sizeof(CStylePropertyHeight) == 24);
static_assert(offsetof(CStylePropertyHeight, m_Height) == WIN64_LINUX(16, 12));

struct CStylePropertyImageShadow : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    bool fullySet;
    CUILength horizontalOffset;
    CUILength verticalOffset;
    CUILength blurRadius;
    float strength;
    Color color;
};
static_assert(std::is_standard_layout_v<CStylePropertyImageShadow>);
static_assert(sizeof(CStylePropertyImageShadow) == WIN64_LINUX(56, 48));
static_assert(offsetof(CStylePropertyImageShadow, fullySet) == WIN64_LINUX(16, 10));
static_assert(offsetof(CStylePropertyImageShadow, horizontalOffset) == WIN64_LINUX(20, 12));
static_assert(offsetof(CStylePropertyImageShadow, verticalOffset) == WIN64_LINUX(28, 20));
static_assert(offsetof(CStylePropertyImageShadow, blurRadius) == WIN64_LINUX(36, 28));
static_assert(offsetof(CStylePropertyImageShadow, strength) == WIN64_LINUX(44, 36));
static_assert(offsetof(CStylePropertyImageShadow, color) == WIN64_LINUX(48, 40));

struct CStylePropertyPosition : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    CUILength x;
    CUILength y;
    CUILength z;
};
static_assert(std::is_standard_layout_v<CStylePropertyPosition>);
static_assert(sizeof(CStylePropertyPosition) == 40);
static_assert(offsetof(CStylePropertyPosition, x) == WIN64_LINUX(16, 12));
static_assert(offsetof(CStylePropertyPosition, y) == WIN64_LINUX(24, 20));
static_assert(offsetof(CStylePropertyPosition, z) == WIN64_LINUX(32, 28));

struct CStylePropertyTransformOrigin : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    CUILength x;
    CUILength y;
    bool m_bParentRelative;
};
static_assert(std::is_standard_layout_v<CStylePropertyTransformOrigin>);
static_assert(sizeof(CStylePropertyTransformOrigin) == WIN64_LINUX(40, 32));
static_assert(offsetof(CStylePropertyTransformOrigin, x) == WIN64_LINUX(16, 12));
static_assert(offsetof(CStylePropertyTransformOrigin, y) == WIN64_LINUX(24, 20));
static_assert(offsetof(CStylePropertyTransformOrigin, m_bParentRelative) == WIN64_LINUX(32, 28));

struct CStylePropertyAlign : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    EHorizontalAlignment m_eHorizontalAlignment;
    EVerticalAlignment m_eVerticalAlignment;
};
static_assert(std::is_standard_layout_v<CStylePropertyAlign>);
static_assert(sizeof(CStylePropertyAlign) == WIN64_LINUX(24, 16));
static_assert(offsetof(CStylePropertyAlign, m_eHorizontalAlignment) == WIN64_LINUX(16, 10));
static_assert(offsetof(CStylePropertyAlign, m_eVerticalAlignment) == WIN64_LINUX(17, 11));

struct CStylePropertyWashColor : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    Color color;
    bool fullySet;
};
static_assert(std::is_standard_layout_v<CStylePropertyWashColor>);
static_assert(sizeof(CStylePropertyWashColor) == WIN64_LINUX(24, 16));
static_assert(offsetof(CStylePropertyWashColor, color) == WIN64_LINUX(16, 10));
static_assert(offsetof(CStylePropertyWashColor, fullySet) == WIN64_LINUX(20, 14));

struct CStylePropertyFlowChildren : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    EFlowDirection m_eFlowDirection;
};
static_assert(std::is_standard_layout_v<CStylePropertyFlowChildren>);
static_assert(sizeof(CStylePropertyFlowChildren) == WIN64_LINUX(24, 16));
static_assert(offsetof(CStylePropertyFlowChildren, m_eFlowDirection) == WIN64_LINUX(16, 10));

struct CStylePropertyFont : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    CUtlString m_strFontFamily;
    float m_flFontSize;
    EFontStyle m_eFontStyle;
    EFontWeight m_eFontWeight;
    EFontStretch m_eFontStretch;
};
static_assert(std::is_standard_layout_v<CStylePropertyFont>);
static_assert(sizeof(CStylePropertyFont) == 32);
static_assert(offsetof(CStylePropertyFont, m_strFontFamily) == 16);
static_assert(offsetof(CStylePropertyFont, m_flFontSize) == 24);
static_assert(offsetof(CStylePropertyFont, m_eFontStyle) == 28);
static_assert(offsetof(CStylePropertyFont, m_eFontWeight) == 29);
static_assert(offsetof(CStylePropertyFont, m_eFontStretch) == 30);

struct CStylePropertyTextShadow : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    bool fullySet;
    CUILength horizontalOffset;
    CUILength verticalOffset;
    CUILength blurRadius;
    float strength;
    Color color;
};
static_assert(std::is_standard_layout_v<CStylePropertyTextShadow>);
static_assert(sizeof(CStylePropertyTextShadow) == WIN64_LINUX(56, 48));
static_assert(offsetof(CStylePropertyTextShadow, fullySet) == WIN64_LINUX(16, 10));
static_assert(offsetof(CStylePropertyTextShadow, horizontalOffset) == WIN64_LINUX(20, 12));
static_assert(offsetof(CStylePropertyTextShadow, verticalOffset) == WIN64_LINUX(28, 20));
static_assert(offsetof(CStylePropertyTextShadow, blurRadius) == WIN64_LINUX(36, 28));
static_assert(offsetof(CStylePropertyTextShadow, strength) == WIN64_LINUX(44, 36));
static_assert(offsetof(CStylePropertyTextShadow, color) == WIN64_LINUX(48, 40));

struct CStylePropertyMixBlendMode : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    EMixBlendMode m_eMixBlendMode;
    bool m_bSet;
};
static_assert(std::is_standard_layout_v<CStylePropertyMixBlendMode>);
static_assert(sizeof(CStylePropertyMixBlendMode) == WIN64_LINUX(24, 16));
static_assert(offsetof(CStylePropertyMixBlendMode, m_eMixBlendMode) == WIN64_LINUX(16, 10));
static_assert(offsetof(CStylePropertyMixBlendMode, m_bSet) == WIN64_LINUX(17, 11));

struct CStylePropertyTextAlign : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    ETextAlign m_eAlign;
};
static_assert(std::is_standard_layout_v<CStylePropertyTextAlign>);
static_assert(sizeof(CStylePropertyTextAlign) == WIN64_LINUX(24, 16));
static_assert(offsetof(CStylePropertyTextAlign, m_eAlign) == WIN64_LINUX(16, 10));

struct CStylePropertyMargin : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    CUILength m_left;
    CUILength m_top;
    CUILength m_right;
    CUILength m_bottom;
};
static_assert(std::is_standard_layout_v<CStylePropertyMargin>);
static_assert(sizeof(CStylePropertyMargin) == 48);
static_assert(offsetof(CStylePropertyMargin, m_left) == WIN64_LINUX(16, 12));
static_assert(offsetof(CStylePropertyMargin, m_top) == WIN64_LINUX(24, 20));
static_assert(offsetof(CStylePropertyMargin, m_right) == WIN64_LINUX(32, 28));
static_assert(offsetof(CStylePropertyMargin, m_bottom) == WIN64_LINUX(40, 36));

struct CFillBrush {
    EStrokeType type;
    union {
        Color fillColor;
        struct CLinearGradient* linearGradient;
        struct CRadialGradient* radialGradient;
    };
    float opacity;
};
static_assert(offsetof(CFillBrush, fillColor) == 8);
static_assert(sizeof(CFillBrush) == 24);

struct CStylePropertyFillColor : CStyleProperty {
};

template <typename T>
struct FixedGrowableVector {
    int numberOfElements;
    int growSize;
    union {
        T* elements;
        T element;
    };
};

#define STYLE_PROPERTY_FILLCOLOR_FIELDS() \
BASE_STYLE_PROPERTY_FIELDS(); \
FixedGrowableVector<CFillBrush> fillBrushes

struct CStylePropertyForegroundColor : CStylePropertyFillColor {
    STYLE_PROPERTY_FILLCOLOR_FIELDS();
};
static_assert(std::is_standard_layout_v<CStylePropertyForegroundColor>);
static_assert(sizeof(CStylePropertyForegroundColor) == 48);
static_assert(offsetof(CStylePropertyForegroundColor, fillBrushes) == 16);

struct CStylePropertyBackgroundColor : CStylePropertyFillColor {
    STYLE_PROPERTY_FILLCOLOR_FIELDS();
    float opacity;
};
static_assert(std::is_standard_layout_v<CStylePropertyBackgroundColor>);
static_assert(sizeof(CStylePropertyBackgroundColor) == 56);
static_assert(offsetof(CStylePropertyBackgroundColor, fillBrushes) == 16);
static_assert(offsetof(CStylePropertyBackgroundColor, opacity) == 48);

struct CStylePropertyTransform3D : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    CUtlVector<CTransform3D*> transforms;
    float cachedParentWidth;
    float cachedParentHeight;
    bool dirty;
    VMatrix matrix;
    bool interpolated;
    bool fullySet;
};
static_assert(std::is_standard_layout_v<CStylePropertyTransform3D>);
static_assert(sizeof(CStylePropertyTransform3D) == 120);
static_assert(offsetof(CStylePropertyTransform3D, transforms) == 16);
static_assert(offsetof(CStylePropertyTransform3D, cachedParentWidth) == 40);
static_assert(offsetof(CStylePropertyTransform3D, cachedParentHeight) == 44);
static_assert(offsetof(CStylePropertyTransform3D, dirty) == 48);
static_assert(offsetof(CStylePropertyTransform3D, matrix) == 52);
static_assert(offsetof(CStylePropertyTransform3D, interpolated) == 116);
static_assert(offsetof(CStylePropertyTransform3D, fullySet) == 117);

struct CStylePropertyBorder : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    EBorderStyle m_rgBorderStyle[4];
    CUILength m_rgBorderWidth[4];
    bool m_rgColorsSet[4];
    Color m_rgBorderColor[4];
};
static_assert(std::is_standard_layout_v<CStylePropertyBorder>);

struct CornerRadii_t {
    CUILength horizontalRadius;
    CUILength verticalRadius;
};

struct CStylePropertyBorderRadius : CStyleProperty {
    BASE_STYLE_PROPERTY_FIELDS();
    CornerRadii_t m_rgCornerRaddi[4];
};
static_assert(std::is_standard_layout_v<CStylePropertyBorderRadius>);

};
