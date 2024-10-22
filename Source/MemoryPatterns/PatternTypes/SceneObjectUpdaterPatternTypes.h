#pragma once

#include <cstdint>

#include <CS2/Classes/SceneObjectUpdaterHandle_t.h>
#include <Utils/FieldOffset.h>

#include "StrongType.h"

template <typename FieldType, typename OffsetType>
using SceneObjectUpdaterOffset = FieldOffset<cs2::SceneObjectUpdaterHandle_t, FieldType, OffsetType>;

STRONG_TYPE(OffsetToSceneObject, SceneObjectUpdaterOffset<cs2::SceneObjectUpdaterHandle_t::sceneObject, std::int8_t>);
