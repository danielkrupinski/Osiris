#pragma once

#include <GameClasses/OffsetTypes/GameRulesOffset.h>

struct GameRulesDeps {
    template <typename GameRulesPatterns>
    explicit GameRulesDeps(const GameRulesPatterns& gameRulesPatterns) noexcept
        : gameRules{gameRulesPatterns.gameRules()}
        , roundStartTimeOffset{gameRulesPatterns.roundStartTimeOffset()}
    {
    }

    [[nodiscard]] static const GameRulesDeps& instance() noexcept;

    cs2::C_CSGameRules** gameRules;
    RoundStartTimeOffset roundStartTimeOffset;
};
