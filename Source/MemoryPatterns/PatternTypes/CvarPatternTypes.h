#pragma once

#include <cstdint>

#include <CS2/Classes/CCvar.h>
#include <Utils/FieldFieldOffset.h>
#include <Utils/StrongTypeAlias.h>

STRONG_TYPE_ALIAS(OffsetToConVarList, FieldFieldOffset<cs2::CCvar, cs2::CCvar::ConVarList, std::int8_t, offsetof(cs2::CCvar::ConVarList, memory)>);
STRONG_TYPE_ALIAS(CvarPointer, cs2::CCvar**);
