#pragma once

#include <cstdint>

#include <CS2/Classes/CCvar.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using CvarOffset = FieldOffset<cs2::CCvar, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToConVarList, CvarOffset<cs2::CCvar::ConVarList, std::int8_t>);
STRONG_TYPE_ALIAS(CvarPointer, cs2::CCvar**);
