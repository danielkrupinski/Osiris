#pragma once

#include <cstdint>

#include <CS2/Classes/CRenderComponent.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using RenderComponentOffset = FieldOffset<cs2::CRenderComponent, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToSceneObjectUpdaters, RenderComponentOffset<cs2::CRenderComponent::sceneObjectUpdaters, std::int8_t>);
