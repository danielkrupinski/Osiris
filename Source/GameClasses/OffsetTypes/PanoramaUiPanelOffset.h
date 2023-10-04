#pragma once

#include <CS2/Classes/Panorama.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using PanoramaUiPanelOffset = FieldOffset<cs2::CUIPanel, FieldType, OffsetType>;
