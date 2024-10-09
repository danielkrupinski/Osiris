#pragma once

#include <cstdint>

#include <CS2/Panorama/CImagePanel.h>
#include <Utils/FieldOffset.h>
#include "StrongType.h"

template <typename FieldType, typename OffsetType>
using PanoramaImagePanelOffset = FieldOffset<cs2::CImagePanel, FieldType, OffsetType>;

STRONG_TYPE(ImagePropertiesOffset, PanoramaImagePanelOffset<cs2::ImageProperties, std::int8_t>);
STRONG_TYPE(OffsetToImagePath, PanoramaImagePanelOffset<cs2::CImagePanel::m_strSource, std::int32_t>);
STRONG_TYPE(SetImageFunctionPointer, cs2::CImagePanel::SetImage*);
STRONG_TYPE(ImagePanelConstructorPointer, cs2::CImagePanel::Constructor*);
STRONG_TYPE(ImagePanelClassSize, std::uint32_t);
