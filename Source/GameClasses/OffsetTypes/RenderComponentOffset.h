#pragma once

#include <cstdint>

#include <CS2/Classes/CRenderComponent.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using RenderComponentOffset = FieldOffset<cs2::CRenderComponent, FieldType, OffsetType>;

using OffsetToSceneObjectUpdaters = RenderComponentOffset<cs2::CRenderComponent::sceneObjectUpdaters, std::int8_t>;
