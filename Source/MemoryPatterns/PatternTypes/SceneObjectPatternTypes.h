#pragma once

#include <cstdint>

#include <CS2/Classes/CSceneObject.h>
#include <Platform/Macros/PlatformSpecific.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using SceneObjectOffset = FieldOffset<cs2::CSceneObject, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToSceneObjectFlags, SceneObjectOffset<cs2::CSceneObject::flags, WIN64_LINUX(std::int32_t, std::int8_t)>);
STRONG_TYPE_ALIAS(OffsetToSceneObjectClass, SceneObjectOffset<cs2::CSceneObject::m_nObjectClass, WIN64_LINUX(std::int32_t, std::int8_t)>);
STRONG_TYPE_ALIAS(OffsetToSceneObjectAttributes, SceneObjectOffset<cs2::CSceneObject::attributes, std::int32_t>);
STRONG_TYPE_ALIAS(OffsetToSceneObjectRenderableFlags, SceneObjectOffset<cs2::CSceneObject::m_nRenderableFlags, std::int32_t>);
