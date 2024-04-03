#pragma once

#include <cstdint>

#include <CS2/Classes/Panorama.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using TopLevelWindowOffset = FieldOffset<cs2::CTopLevelWindow, FieldType, OffsetType>;

using UiScaleFactorOffset = TopLevelWindowOffset<cs2::CTopLevelWindow::m_flScaleFactor, std::int8_t>;
