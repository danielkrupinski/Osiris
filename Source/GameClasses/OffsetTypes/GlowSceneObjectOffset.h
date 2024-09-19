#pragma once

#include <cstdint>

#include <CS2/Classes/Glow.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using GlowSceneObjectOffset = FieldOffset<cs2::CGlowHelperSceneObject, FieldType, OffsetType>;

using OffsetToGlowSceneObjectEntity = GlowSceneObjectOffset<cs2::CGlowHelperSceneObject::entity, std::int32_t>;
using OffsetToGlowSceneObjectAttachedSceneObject = GlowSceneObjectOffset<cs2::CGlowHelperSceneObject::attachedSceneObject, std::int32_t>;
