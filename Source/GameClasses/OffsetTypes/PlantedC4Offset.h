#pragma once

#include <cstdint>

#include <CS2/Classes/CPlantedC4.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using PlantedC4Offset = FieldOffset<cs2::CPlantedC4, FieldType, OffsetType>;

using BombSiteOffset = PlantedC4Offset<cs2::CPlantedC4::m_nBombSite, std::int32_t>;
using BombTickingOffset = PlantedC4Offset<cs2::CPlantedC4::m_bBombTicking, std::int32_t>;
using BombBlowTimeOffset = PlantedC4Offset<cs2::CPlantedC4::m_flC4Blow, std::int32_t>;
using BombDefuserOffset = PlantedC4Offset<cs2::CPlantedC4::m_hBombDefuser, std::int32_t>;
using BombDefuseEndTimeOffset = PlantedC4Offset<cs2::CPlantedC4::m_flDefuseCountDown, std::int32_t>;
