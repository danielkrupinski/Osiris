#pragma once

#include <CS2/Classes/C_CSGameRules.h>
#include <GameClasses/OffsetTypes/GameRulesOffset.h>

using RoundStartTimeOffset = GameRulesOffset<cs2::C_CSGameRules::m_fRoundStartTime, std::int8_t>;

struct GameRulesPatterns {
    [[nodiscard]] static RoundStartTimeOffset roundStartTimeOffset() noexcept;
};
