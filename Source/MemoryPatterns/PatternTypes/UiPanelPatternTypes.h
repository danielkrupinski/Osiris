#pragma once

#include <cstdint>

#include <CS2/Panorama/CPanelStyle.h>
#include <CS2/Panorama/CTopLevelWindow.h>
#include <CS2/Panorama/CUIPanel.h>
#include <Utils/FieldOffset.h>
#include "StrongType.h"

template <typename FieldType, typename OffsetType>
using PanoramaUiPanelOffset = FieldOffset<cs2::CUIPanel, FieldType, OffsetType>;

STRONG_TYPE(SetParentFunctionOffset, FieldOffset<const void, cs2::CUIPanel::setParent, std::int32_t>);
STRONG_TYPE(SetVisibleFunctionOffset, FieldOffset<const void, cs2::CUIPanel::setVisible, std::int32_t>);
STRONG_TYPE(GetAttributeStringFunctionOffset, FieldOffset<const void, cs2::CUIPanel::getAttributeString, std::int32_t>);
STRONG_TYPE(SetAttributeStringFunctionOffset, FieldOffset<const void, cs2::CUIPanel::setAttributeString, std::int32_t>);

STRONG_TYPE(ChildPanelsVectorOffset, PanoramaUiPanelOffset<cs2::CUIPanel::childrenVector, std::int8_t>);
STRONG_TYPE(PanelClassesVectorOffset, PanoramaUiPanelOffset<cs2::CUIPanel::classesVector, std::int32_t>);
STRONG_TYPE(PanelStyleOffset, PanoramaUiPanelOffset<cs2::CPanelStyle, std::int8_t>);
STRONG_TYPE(ParentWindowOffset, PanoramaUiPanelOffset<cs2::CTopLevelWindow*, std::int8_t>);
STRONG_TYPE(OffsetToPanelId, PanoramaUiPanelOffset<cs2::CUIPanel::m_pchID, std::int8_t>);
STRONG_TYPE(OffsetToPanelFlags, PanoramaUiPanelOffset<cs2::CUIPanel::PanelFlags, std::int32_t>);
