#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/WeaponEntities.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using C4Offset = FieldOffset<cs2::C_C4, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToIsBeingPlanted, C4Offset<cs2::C_C4::m_bStartedArming, std::int32_t>);
