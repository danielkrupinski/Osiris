#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/C_BaseModelEntity.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using BaseModelEntityOffset = FieldOffset<cs2::C_BaseModelEntity, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToGlowProperty, BaseModelEntityOffset<cs2::C_BaseModelEntity::m_Glow, std::int32_t>);
