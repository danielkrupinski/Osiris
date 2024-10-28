#pragma once

#include <cstdint>

#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using EntityOffset = FieldOffset<cs2::C_BaseEntity, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToGameSceneNode, EntityOffset<cs2::C_BaseEntity::m_pGameSceneNode, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToHealth, EntityOffset<cs2::C_BaseEntity::m_iHealth, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToLifeState, EntityOffset<cs2::C_BaseEntity::m_lifeState, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToTeamNumber, EntityOffset<cs2::C_BaseEntity::m_iTeamNum, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToVData, EntityOffset<cs2::C_BaseEntity::m_pSubclassVData, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToRenderComponent, EntityOffset<cs2::C_BaseEntity::m_pRenderComponent, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToOwnerEntity, EntityOffset<cs2::C_BaseEntity::m_hOwnerEntity, std::int32_t>);
