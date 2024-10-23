#pragma once

#include <cstdint>

#include <CS2/Panorama/CPanelStyle.h>
#include <CS2/Panorama/CTopLevelWindow.h>
#include <CS2/Panorama/CUIPanel.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using PanoramaUiPanelOffset = FieldOffset<cs2::CUIPanel, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(SetParentFunctionOffset, FieldOffset<const void, cs2::CUIPanel::setParent, std::int32_t>);
STRONG_TYPE_ALIAS(SetVisibleFunctionOffset, FieldOffset<const void, cs2::CUIPanel::setVisible, std::int32_t>);
STRONG_TYPE_ALIAS(GetAttributeStringFunctionOffset, FieldOffset<const void, cs2::CUIPanel::getAttributeString, std::int32_t>);
STRONG_TYPE_ALIAS(SetAttributeStringFunctionOffset, FieldOffset<const void, cs2::CUIPanel::setAttributeString, std::int32_t>);

STRONG_TYPE_ALIAS(ChildPanelsVectorOffset, PanoramaUiPanelOffset<cs2::CUIPanel::childrenVector, std::int8_t>);
STRONG_TYPE_ALIAS(PanelClassesVectorOffset, PanoramaUiPanelOffset<cs2::CUIPanel::classesVector, std::int32_t>);
STRONG_TYPE_ALIAS(PanelStyleOffset, PanoramaUiPanelOffset<cs2::CPanelStyle, std::int8_t>);
STRONG_TYPE_ALIAS(ParentWindowOffset, PanoramaUiPanelOffset<cs2::CTopLevelWindow*, std::int8_t>);
STRONG_TYPE_ALIAS(OffsetToPanelId, PanoramaUiPanelOffset<cs2::CUIPanel::m_pchID, std::int8_t>);
STRONG_TYPE_ALIAS(OffsetToPanelFlags, PanoramaUiPanelOffset<cs2::CUIPanel::PanelFlags, std::int32_t>);
