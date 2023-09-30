#pragma once

#include <CS2/Classes/C_CSGameRules.h>
#include <Utils/FieldOffset.h>

template <typename FieldType, typename OffsetType>
using GameRulesOffset = FieldOffset<cs2::C_CSGameRules, FieldType, OffsetType>;
