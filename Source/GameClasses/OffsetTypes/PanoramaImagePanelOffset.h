#pragma once

#include <CS2/Classes/Panorama.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using PanoramaImagePanelOffset = FieldOffset<cs2::CImagePanel, FieldType, OffsetType>;
