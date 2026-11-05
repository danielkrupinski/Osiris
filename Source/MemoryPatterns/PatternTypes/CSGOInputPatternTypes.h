#pragma once

#include <cstdint>

#include <CS2/Classes/CCSGOInput.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using CSGOInputOffset = FieldOffset<cs2::CCSGOInput, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(CSGOInputPointer, cs2::CCSGOInput**);
STRONG_TYPE_ALIAS(OffsetToViewAngles, CSGOInputOffset<cs2::CCSGOInput::m_angViewAngles, std::int32_t>);
