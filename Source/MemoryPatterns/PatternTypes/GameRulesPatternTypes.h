#pragma once

#include <cstdint>
#include <CS2/Classes/C_CSGameRules.h>
#include <Utils/FieldOffset.h>

#include "StrongType.h"

template <typename FieldType, typename OffsetType>
using GameRulesOffset = FieldOffset<cs2::C_CSGameRules, FieldType, OffsetType>;

STRONG_TYPE(GameRulesPointer, cs2::C_CSGameRules**);
STRONG_TYPE(RoundStartTimeOffset, GameRulesOffset<cs2::C_CSGameRules::m_fRoundStartTime, std::int8_t>);
STRONG_TYPE(OffsetToRoundRestartTime, GameRulesOffset<cs2::C_CSGameRules::m_flRestartRoundTime, std::int8_t>);
