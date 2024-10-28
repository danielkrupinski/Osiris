#pragma once

#include <cstdint>

#include <CS2/Classes/Glow.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using GlowSceneObjectOffset = FieldOffset<cs2::CGlowHelperSceneObject, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToGlowSceneObjectEntity, GlowSceneObjectOffset<cs2::CGlowHelperSceneObject::entity, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToGlowSceneObjectAttachedSceneObject, GlowSceneObjectOffset<cs2::CGlowHelperSceneObject::attachedSceneObject, std::int32_t>);
