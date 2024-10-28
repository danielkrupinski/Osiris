#pragma once

#include <cstdint>

#include <CS2/Panorama/CTopLevelWindow.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using TopLevelWindowOffset = FieldOffset<cs2::CTopLevelWindow, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(UiScaleFactorOffset, TopLevelWindowOffset<cs2::CTopLevelWindow::m_flScaleFactor, std::int8_t>);
