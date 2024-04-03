#pragma once

#include <cstdint>

#include <CS2/Classes/Panorama.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using PanoramaUiPanelOffset = FieldOffset<cs2::CUIPanel, FieldType, OffsetType>;

using ChildPanelsVectorOffset = PanoramaUiPanelOffset<cs2::CUIPanel::childrenVector, std::int8_t>;
using PanelClassesVectorOffset = PanoramaUiPanelOffset<cs2::CUIPanel::classesVector, std::int32_t>;
using PanelStyleOffset = PanoramaUiPanelOffset<cs2::CPanelStyle, std::int8_t>;
using ParentWindowOffset = PanoramaUiPanelOffset<cs2::CTopLevelWindow*, std::int8_t>;
