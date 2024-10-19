#pragma once

#include <cstdint>

#include <CS2/Panorama/CUIPanel.h>
#include <Utils/FieldOffset.h>
#include "StrongType.h"

STRONG_TYPE(SetParentFunctionOffset, FieldOffset<const void, cs2::CUIPanel::setParent, std::int32_t>);
STRONG_TYPE(SetVisibleFunctionOffset, FieldOffset<const void, cs2::CUIPanel::setVisible, std::int32_t>);
STRONG_TYPE(GetAttributeStringFunctionOffset, FieldOffset<const void, cs2::CUIPanel::getAttributeString, std::int32_t>);
STRONG_TYPE(SetAttributeStringFunctionOffset, FieldOffset<const void, cs2::CUIPanel::setAttributeString, std::int32_t>);
