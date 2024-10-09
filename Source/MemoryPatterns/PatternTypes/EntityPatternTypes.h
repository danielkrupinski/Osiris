#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <Utils/FieldOffset.h>

#include "StrongType.h"

template <typename FieldType, typename OffsetType>
using EntityOffset = FieldOffset<cs2::C_BaseEntity, FieldType, OffsetType>;

STRONG_TYPE(OffsetToGameSceneNode, EntityOffset<cs2::C_BaseEntity::m_pGameSceneNode, std::int32_t>);
STRONG_TYPE(OffsetToHealth, EntityOffset<cs2::C_BaseEntity::m_iHealth, std::int32_t>);
STRONG_TYPE(OffsetToLifeState, EntityOffset<cs2::C_BaseEntity::m_lifeState, std::int32_t>);
STRONG_TYPE(OffsetToTeamNumber, EntityOffset<cs2::C_BaseEntity::m_iTeamNum, std::int32_t>);
STRONG_TYPE(OffsetToVData, EntityOffset<cs2::C_BaseEntity::m_pSubclassVData, std::int32_t>);
STRONG_TYPE(OffsetToRenderComponent, EntityOffset<cs2::C_BaseEntity::m_pRenderComponent, std::int32_t>);
STRONG_TYPE(OffsetToOwnerEntity, EntityOffset<cs2::C_BaseEntity::m_hOwnerEntity, std::int32_t>);
