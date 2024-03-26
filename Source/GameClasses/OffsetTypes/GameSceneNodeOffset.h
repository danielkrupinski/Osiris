#pragma once

#include <CS2/Classes/CGameSceneNode.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using GameSceneNodeOffset = FieldOffset<cs2::CGameSceneNode, FieldType, OffsetType>;
