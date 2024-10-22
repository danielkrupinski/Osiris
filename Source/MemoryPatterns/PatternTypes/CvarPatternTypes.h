#pragma once

#include <CS2/Classes/CCvar.h>
#include <Utils/FieldOffset.h>

#include "StrongType.h"

template <typename FieldType, typename OffsetType>
using CvarOffset = FieldOffset<cs2::CCvar, FieldType, OffsetType>;

STRONG_TYPE(OffsetToConVarList, CvarOffset<cs2::CCvar::ConVarList, std::int8_t>);
STRONG_TYPE(CvarPointer, cs2::CCvar**);
