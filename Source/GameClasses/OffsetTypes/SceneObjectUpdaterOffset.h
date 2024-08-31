#pragma once

#include <cstdint>

#include <CS2/Classes/SceneObjectUpdaterHandle_t.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using SceneObjectUpdaterOffset = FieldOffset<cs2::SceneObjectUpdaterHandle_t, FieldType, OffsetType>;

using OffsetToSceneObject = SceneObjectUpdaterOffset<cs2::SceneObjectUpdaterHandle_t::sceneObject, std::int8_t>;
