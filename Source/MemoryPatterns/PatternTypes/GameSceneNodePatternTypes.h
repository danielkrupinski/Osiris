#pragma once

#include <cstdint>

#include <CS2/Classes/CGameSceneNode.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using GameSceneNodeOffset = FieldOffset<cs2::CGameSceneNode, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToAbsOrigin, GameSceneNodeOffset<cs2::CGameSceneNode::m_vecAbsOrigin, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToGameSceneNodeOwner, GameSceneNodeOffset<cs2::CGameSceneNode::m_pOwner, std::int8_t>);
STRONG_TYPE_ALIAS(OffsetToChildGameSceneNode, GameSceneNodeOffset<cs2::CGameSceneNode::m_pChild, std::int8_t>);
STRONG_TYPE_ALIAS(OffsetToNextSiblingGameSceneNode, GameSceneNodeOffset<cs2::CGameSceneNode::m_pNextSibling, std::int8_t>);
