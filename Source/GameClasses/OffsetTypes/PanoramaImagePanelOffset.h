#pragma once

#include <cstdint>

#include <CS2/Classes/Panorama.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using PanoramaImagePanelOffset = FieldOffset<cs2::CImagePanel, FieldType, OffsetType>;

using ImagePropertiesOffset = PanoramaImagePanelOffset<cs2::ImageProperties, std::int8_t>;
using OffsetToImagePath = PanoramaImagePanelOffset<cs2::CImagePanel::m_strSource, std::int32_t>;
