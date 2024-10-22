#pragma once

#include <cstdint>

#include <CS2/Classes/CSceneObject.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Utils/FieldOffset.h>

#include "StrongType.h"

template <typename FieldType, typename OffsetType>
using SceneObjectOffset = FieldOffset<cs2::CSceneObject, FieldType, OffsetType>;

STRONG_TYPE(OffsetToSceneObjectFlags, SceneObjectOffset<cs2::CSceneObject::flags, WIN64_LINUX(std::int32_t, std::int8_t)>);
STRONG_TYPE(OffsetToSceneObjectClass, SceneObjectOffset<cs2::CSceneObject::m_nObjectClass, WIN64_LINUX(std::int32_t, std::int8_t)>);
