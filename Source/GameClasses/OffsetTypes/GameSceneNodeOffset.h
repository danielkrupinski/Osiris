#pragma once

#include <cstdint>

#include <CS2/Classes/CGameSceneNode.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using GameSceneNodeOffset = FieldOffset<cs2::CGameSceneNode, FieldType, OffsetType>;

using OffsetToAbsOrigin = GameSceneNodeOffset<cs2::CGameSceneNode::m_vecAbsOrigin, std::int32_t>;
