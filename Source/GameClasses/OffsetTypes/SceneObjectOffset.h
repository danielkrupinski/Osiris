#pragma once

#include <cstdint>

#include <CS2/Classes/CSceneObject.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using SceneObjectOffset = FieldOffset<cs2::CSceneObject, FieldType, OffsetType>;

using OffsetToSceneObjectFlags = SceneObjectOffset<cs2::CSceneObject::flags, WIN64_LINUX(std::int32_t, std::int8_t)>;
using OffsetToSceneObjectClass = SceneObjectOffset<cs2::CSceneObject::m_nObjectClass, WIN64_LINUX(std::int32_t, std::int8_t)>;
