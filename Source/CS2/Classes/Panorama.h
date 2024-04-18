#pragma once

#include <cstdint>

#include <Platform/Macros/IsPlatform.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Utils/Pad.h>

#include "Color.h"
#include "CUtlMap.h"
#include "CUtlString.h"
#include "CUtlVector.h"
#include "Vector.h"

namespace cs2
{

using CPanoramaSymbol = short;

struct PanelHandle {
    std::int32_t panelIndex = -1;
    std::uint32_t serialNumber;

    [[nodiscard]] bool isValid() const noexcept
    {
        return panelIndex != -1;
    }

    [[nodiscard]] constexpr bool operator==(const PanelHandle&) const = default;
};

struct CUIPanel;

struct CUIEngine {
#if IS_WIN64()
    using getPanelHandle = void (*)(CUIEngine* thisptr, PanelHandle* handle, CUIPanel* panel);
#elif IS_LINUX()
    using getPanelHandle = PanelHandle (*)(CUIEngine* thisptr, CUIPanel* panel);
#endif
    using getPanelPointer = CUIPanel* (*)(CUIEngine* thisptr, const PanelHandle* handle);
    using runScript = void (*)(CUIEngine* thisptr, CUIPanel* contextPanel, const char* scriptSource, const char* originFile, std::uint64_t line);
    using makeSymbol = CPanoramaSymbol (*)(CUIEngine* thisptr, int type, const char* text);
    using onDeletePanel = void (*)(CUIEngine* thisptr, PanelHandle* panelHandle);
};

struct CPanel2D {
    const void* vmt;
    CUIPanel* uiPanel;

    using Create = CPanel2D*(const char* id, CUIPanel* parent);
};

struct CLabel : CPanel2D {
    using Constructor = void(CLabel* thisptr, CPanel2D* parent, const char* id);
    using setTextInternal = void (*)(CLabel* thisptr, const char* value, int textType, bool trustedSource);
};

struct CStyleSymbol {
    static constexpr auto kInvalidId{0xFF};

    [[nodiscard]] bool isValid() const noexcept
    {
        return m_Id != kInvalidId;
    }

    std::uint8_t m_Id{kInvalidId};
};

struct CStyleProperty {
    const void* vmt;
    CStyleSymbol m_symPropertyName;
    bool m_bDisallowTransition;
};

struct CUILength {
    enum EUILengthTypes {
        k_EUILengthUnset,
        k_EUILengthLength,
        k_EUILengthPercent
    };

    [[nodiscard]] static CUILength pixels(float value) noexcept
    {
        return CUILength{value, k_EUILengthLength};
    }

    [[nodiscard]] static CUILength percent(float value) noexcept
    {
        return CUILength{value, k_EUILengthPercent};
    }

    float m_flValue;
    EUILengthTypes m_eType;
};

struct CStylePropertyWidth : CStyleProperty {
    static constexpr auto kName{"width"};
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCStylePropertyWidth@panorama@@", "N8panorama19CStylePropertyWidthE")};

    CUILength m_Length;
};

struct CStylePropertyOpacity : CStyleProperty {
    static constexpr auto kName{"opacity"};
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCStylePropertyOpacity@panorama@@", "N8panorama21CStylePropertyOpacityE")};

    float opacity;
};

struct CStylePropertyZIndex : CStyleProperty {
    static constexpr auto kName{"z-index"};
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCStylePropertyZIndex@panorama@@", "N8panorama20CStylePropertyZIndexE")};

    float zindex;
};

struct CStylePropertyHeight : CStyleProperty {
    static constexpr auto kName{"height"};
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCStylePropertyHeight@panorama@@", "N8panorama20CStylePropertyHeightE")};

    CUILength m_Height;
};

struct CStylePropertyImageShadow : CStyleProperty {
    static constexpr auto kName{"img-shadow"};
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCStylePropertyImageShadow@panorama@@", "N8panorama25CStylePropertyImageShadowE")};

    bool fullySet;
    CUILength horizontalOffset;
    CUILength verticalOffset;
    CUILength blurRadius;
    float strength;
    Color color;
};

struct CStylePropertyPosition : CStyleProperty {
    static constexpr auto kName{"position"};
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCStylePropertyPosition@panorama@@", "N8panorama22CStylePropertyPositionE")};

    CUILength x;
    CUILength y;
    CUILength z;
};

struct CStylePropertyTransformOrigin : CStyleProperty {
    static constexpr auto kName{"transform-origin"};
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCStylePropertyTransformOrigin@panorama@@", "N8panorama29CStylePropertyTransformOriginE")};

    CUILength x;
    CUILength y;
    bool m_bParentRelative;
};

enum EHorizontalAlignment : std::uint8_t {
	k_EHorizontalAlignmentUnset,
	k_EHorizontalAlignmentLeft,
	k_EHorizontalAlignmentCenter,
	k_EHorizontalAlignmentRight
};

enum EVerticalAlignment : std::uint8_t {
	k_EVerticalAlignmentUnset,
	k_EVerticalAlignmentTop,
	k_EVerticalAlignmentCenter,
	k_EVerticalAlignmentBottom
};

struct CStylePropertyAlign : CStyleProperty {
    static constexpr auto kName{"align"};
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCStylePropertyAlign@panorama@@", "N8panorama19CStylePropertyAlignE")};

    EHorizontalAlignment m_eHorizontalAlignment;
    EVerticalAlignment m_eVerticalAlignment;
};

struct CStylePropertyWashColor : CStyleProperty {
    static constexpr auto kName{"wash-color"};
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCStylePropertyWashColor@panorama@@", "N8panorama23CStylePropertyWashColorE")};

    Color color;
    bool fullySet;
};

enum EFlowDirection : std::uint8_t {
    k_EFlowNone = 1,
    k_EFlowDown,
    k_EFlowRight
};

struct CStylePropertyFlowChildren : CStyleProperty {
    static constexpr auto kName{"flow-children"};
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCStylePropertyFlowChildren@panorama@@", "N8panorama26CStylePropertyFlowChildrenE")};

    EFlowDirection m_eFlowDirection;
};

enum EFontStyle : std::int8_t {
    k_EFontStyleUnset = -1,
    k_EFontStyleNormal = 0,
    k_EFontStyleItalic = 2,
};

enum EFontWeight : std::int8_t {
    k_EFontWeightUnset = -1,
    k_EFontWeightNormal = 0,
    k_EFontWeightMedium = 1,
    k_EFontWeightBold = 2,
    k_EFontWeightBlack = 3,
    k_EFontWeightThin = 4,
    k_EFontWeightLight = 5,
    k_EFontWeightSemiBold = 6,
};

enum EFontStretch : std::int8_t {
    k_EFontStretchUnset = -1,
    k_EFontStretchNormal = 0,
    k_EFontStretchCondensed = 1,
    k_EFontStretchExpanded = 2
};

struct CStylePropertyFont : CStyleProperty {
    static constexpr auto kName{"font"};
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCStylePropertyFont@panorama@@", "N8panorama18CStylePropertyFontE")};

    CUtlString m_strFontFamily;
    float m_flFontSize;
    EFontStyle m_eFontStyle;
    EFontWeight m_eFontWeight;
    EFontStretch m_eFontStretch;
};

struct CStylePropertyTextShadow : CStyleProperty {
    static constexpr auto kName{"text-shadow"};
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCStylePropertyTextShadow@panorama@@", "N8panorama24CStylePropertyTextShadowE")};

    bool fullySet;
    CUILength horizontalOffset;
    CUILength verticalOffset;
    CUILength blurRadius;
    float strength;
    Color color;
};

struct CStylePropertyDimensionsBase : CStyleProperty {
    CUILength m_left;
    CUILength m_top;
    CUILength m_right;
    CUILength m_bottom;
};

struct CStylePropertyMargin : CStylePropertyDimensionsBase {
    static constexpr auto kName{"margin"};
    static constexpr auto kMangledTypeName{WIN32_LINUX(".?AVCStylePropertyMargin@panorama@@", "N8panorama20CStylePropertyMarginE")};
};

struct CPanelStyle {
    using SetProperty = void(CPanelStyle* thisptr, CStyleProperty* styleProperty, bool transition);

    using StylePropertySymbols = CUtlMap<CUtlString, CStyleSymbol>;
};

struct CUIPanel {
    const void* vmt;
    CPanel2D* clientPanel;

    using setParent = void (*)(CUIPanel* thisptr, CUIPanel* parent);
    using setVisible = void (*)(CUIPanel* thisptr, bool visible);
    using findChildInLayoutFile = CUIPanel* (*)(CUIPanel* thisptr, const char* childId);
    using getAttributeString = const char* (*)(CUIPanel* thisptr, CPanoramaSymbol attributeName, const char* defaultValue);
    using setAttributeString = void (*)(CUIPanel* thisptr, CPanoramaSymbol attributeName, const char* value);

    using childrenVector = CUtlVector<CUIPanel*>;
    using classesVector = CUtlVector<CPanoramaSymbol>;
};

struct ImageProperties {
    PAD(16);
    int textureWidth;
    int textureHeight;
    float scale;
};

struct CImagePanel : CPanel2D {
    using m_strSource = CUtlString;

    using Constructor = void(CImagePanel* thisptr, CPanel2D* parent, const char* id);
    using SetImage = void(CImagePanel* thisptr, const char* imageUrl, const char* defaultImageUrl, ImageProperties* properties);
};

struct CTransform3D {
    const void* vmt;
};

struct CTransformTranslate3D : CTransform3D {
    CUILength m_x;
    CUILength m_y;
    CUILength m_z;
};

struct CTransformScale3D : CTransform3D {
    Vector m_VecScale;
};

struct CTopLevelWindow {
    using m_flScaleFactor = float;
};

}
