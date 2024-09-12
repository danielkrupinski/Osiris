#pragma once

#include <cstdint>

#include <CS2/Classes/CGameSceneNode.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using GameSceneNodeOffset = FieldOffset<cs2::CGameSceneNode, FieldType, OffsetType>;

using OffsetToAbsOrigin = GameSceneNodeOffset<cs2::CGameSceneNode::m_vecAbsOrigin, std::int32_t>;
using OffsetToGameSceneNodeOwner = GameSceneNodeOffset<cs2::CGameSceneNode::m_pOwner, std::int8_t>;
using OffsetToChildGameSceneNode = GameSceneNodeOffset<cs2::CGameSceneNode::m_pChild, std::int8_t>;
using OffsetToNextSiblingGameSceneNode = GameSceneNodeOffset<cs2::CGameSceneNode::m_pNextSibling, std::int8_t>;
