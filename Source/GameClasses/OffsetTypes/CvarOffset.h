#pragma once

#include <cstdint>

#include <CS2/Classes/CCvar.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using CvarOffset = FieldOffset<cs2::CCvar, FieldType, OffsetType>;

using OffsetToConVarList = CvarOffset<cs2::CCvar::ConVarList, std::int8_t>;
