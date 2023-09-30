#pragma once

#include <CS2/Classes/CPlantedC4.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using PlantedC4Offset = FieldOffset<cs2::CPlantedC4, FieldType, OffsetType>;
