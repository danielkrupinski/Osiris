#pragma once

#include <cstdint>

#include <CS2/Classes/ConVar.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using ConVarOffset = FieldOffset<cs2::ConVar, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToConVarValueType, ConVarOffset<cs2::ConVar::ValueType, std::int8_t>);
STRONG_TYPE_ALIAS(OffsetToConVarValue, ConVarOffset<cs2::ConVar::Value, std::int8_t>);
