#pragma once

#include <cstdint>

#include <CS2/Classes/CGlowProperty.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using GlowPropertyOffset = FieldOffset<cs2::CGlowProperty, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToIsGlowing, GlowPropertyOffset<cs2::CGlowProperty::m_bGlowing, std::int8_t>);
