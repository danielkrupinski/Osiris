#pragma once

#include <CS2/Classes/ClientModeCSNormal.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using ClientModeOffset = FieldOffset<cs2::ClientModeCSNormal, FieldType, OffsetType>;
