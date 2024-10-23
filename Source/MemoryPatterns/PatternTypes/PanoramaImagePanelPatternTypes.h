#pragma once

#include <cstdint>

#include <CS2/Panorama/CImagePanel.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using PanoramaImagePanelOffset = FieldOffset<cs2::CImagePanel, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(ImagePropertiesOffset, PanoramaImagePanelOffset<cs2::ImageProperties, std::int8_t>);
STRONG_TYPE_ALIAS(OffsetToImagePath, PanoramaImagePanelOffset<cs2::CImagePanel::m_strSource, std::int32_t>);
STRONG_TYPE_ALIAS(SetImageFunctionPointer, cs2::CImagePanel::SetImage*);
STRONG_TYPE_ALIAS(ImagePanelConstructorPointer, cs2::CImagePanel::Constructor*);
STRONG_TYPE_ALIAS(ImagePanelClassSize, std::uint32_t);
