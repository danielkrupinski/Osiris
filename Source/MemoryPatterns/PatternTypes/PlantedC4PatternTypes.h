#pragma once

#include <cstdint>

#include <CS2/Classes/CPlantedC4.h>
#include <CS2/Classes/CUtlVector.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using PlantedC4Offset = FieldOffset<cs2::CPlantedC4, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(PlantedC4sPointer, cs2::CUtlVector<cs2::CPlantedC4*>*);
STRONG_TYPE_ALIAS(BombSiteOffset, PlantedC4Offset<cs2::CPlantedC4::m_nBombSite, std::int32_t>);
STRONG_TYPE_ALIAS(BombTickingOffset, PlantedC4Offset<cs2::CPlantedC4::m_bBombTicking, std::int32_t>);
STRONG_TYPE_ALIAS(BombBlowTimeOffset, PlantedC4Offset<cs2::CPlantedC4::m_flC4Blow, std::int32_t>);
STRONG_TYPE_ALIAS(BombDefuserOffset, PlantedC4Offset<cs2::CPlantedC4::m_hBombDefuser, std::int32_t>);
STRONG_TYPE_ALIAS(BombDefuseEndTimeOffset, PlantedC4Offset<cs2::CPlantedC4::m_flDefuseCountDown, std::int32_t>);
