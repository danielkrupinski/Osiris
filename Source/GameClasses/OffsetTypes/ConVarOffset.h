#pragma once

#include <cstdint>

#include <CS2/Classes/ConVar.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using ConVarOffset = FieldOffset<cs2::ConVar, FieldType, OffsetType>;

using OffsetToConVarValueType = ConVarOffset<cs2::ConVar::ValueType, std::int8_t>;
using OffsetToConVarValue = ConVarOffset<cs2::ConVar::Value, std::int8_t>;
