#pragma once

#include <CS2/Classes/Panorama.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using TopLevelWindowOffset = FieldOffset<cs2::CTopLevelWindow, FieldType, OffsetType>;
