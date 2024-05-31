#pragma once

#include <GameClasses/OffsetTypes/GameRulesOffset.h>

struct GameRulesImpl {
    template <typename GameRulesPatterns>
    explicit GameRulesImpl(const GameRulesPatterns& gameRulesPatterns) noexcept
        : gameRules{gameRulesPatterns.gameRules()}
        , roundStartTimeOffset{gameRulesPatterns.roundStartTimeOffset()}
    {
    }

    [[nodiscard]] static const GameRulesImpl& instance() noexcept;

    cs2::C_CSGameRules** gameRules;
    RoundStartTimeOffset roundStartTimeOffset;
};
