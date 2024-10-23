#pragma once

#include <cstdint>
#include <CS2/Classes/C_CSGameRules.h>
#include <Utils/FieldOffset.h>
#include <Utils/StrongTypeAlias.h>

template <typename FieldType, typename OffsetType>
using GameRulesOffset = FieldOffset<cs2::C_CSGameRules, FieldType, OffsetType>;

STRONG_TYPE_ALIAS(GameRulesPointer, cs2::C_CSGameRules**);
STRONG_TYPE_ALIAS(RoundStartTimeOffset, GameRulesOffset<cs2::C_CSGameRules::m_fRoundStartTime, std::int8_t>);
STRONG_TYPE_ALIAS(OffsetToRoundRestartTime, GameRulesOffset<cs2::C_CSGameRules::m_flRestartRoundTime, std::int8_t>);
