#pragma once

#include <cstdint>

namespace cs2
{

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

enum EFlowDirection : std::uint8_t {
    k_EFlowNone = 1,
    k_EFlowDown,
    k_EFlowRight
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

enum EMixBlendMode : std::uint8_t {
    k_EMixBlendModeNormal,
    k_EMixBlendModeMultiply,
    k_EMixBlendModeScreen,
    k_EMixBlendModeAdditive,
    k_EMixBlendModeOpaque
};

enum ETextAlign : std::int8_t {
    k_ETextAlignUnset = -1,
    k_ETextAlignLeft = 0,
    k_ETextAlignCenter = 1,
    k_ETextAlignRight = 2,
    k_ETextAlignJustify = 3,
    k_ETextAlignJustifyLetterSpacing = 4
};

}
