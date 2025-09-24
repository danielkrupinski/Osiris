#pragma once

#include <cstdint>

#include <CS2/Classes/GlobalVars.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using GlobalVarsOffset = FieldOffset<cs2::GlobalVars, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(OffsetToFrametime, GlobalVarsOffset<cs2::GlobalVars::frametime, std::int8_t>);
