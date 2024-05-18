#pragma once

#include <GameClasses/OffsetTypes/GameRulesOffset.h>

struct GameRulesImpl {
    template <typename GameRulesPatterns>
    explicit GameRulesImpl(const GameRulesPatterns& gameRulesPatterns) noexcept
        : roundStartTimeOffset{gameRulesPatterns.roundStartTimeOffset()}
    {
    }

    [[nodiscard]] static const GameRulesImpl& instance() noexcept;

    RoundStartTimeOffset roundStartTimeOffset;
};
