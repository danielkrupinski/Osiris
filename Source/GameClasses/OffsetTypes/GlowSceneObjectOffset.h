#pragma once

#include <cstdint>

#include <CS2/Classes/Glow.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using GlowSceneObjectOffset = FieldOffset<cs2::CGlowHelperSceneObject, FieldType, OffsetType>;

using OffsetToGlowSceneObjectEntity = GlowSceneObjectOffset<cs2::CGlowHelperSceneObject::entity, std::int32_t>;
using OffsetToSceneObjectFlags = GlowSceneObjectOffset<cs2::CGlowHelperSceneObject::flags, WIN64_LINUX(std::int32_t, std::int8_t)>;
