#pragma once

#include <GameClasses/OffsetTypes/GameRulesOffset.h>

struct GameRulesDeps {
    GameRulesDeps() = default;

    template <typename GameRulesPatterns>
    explicit GameRulesDeps(const GameRulesPatterns& gameRulesPatterns) noexcept
        : gameRules{gameRulesPatterns.gameRules()}
        , roundStartTimeOffset{gameRulesPatterns.roundStartTimeOffset()}
        , offsetToRoundRestartTime{gameRulesPatterns.offsetToRoundRestartTime()}
    {
    }

    cs2::C_CSGameRules** gameRules;
    RoundStartTimeOffset roundStartTimeOffset;
    OffsetToRoundRestartTime offsetToRoundRestartTime;
};
