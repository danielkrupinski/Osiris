#pragma once

#include <MemoryPatterns/GameRulesPatterns.h>

struct GameRulesImpl {
    explicit GameRulesImpl(const GameRulesPatterns& gameRulesPatterns) noexcept
        : roundStartTimeOffset{gameRulesPatterns.roundStartTimeOffset()}
    {
    }

    [[nodiscard]] static const GameRulesImpl& instance() noexcept;

    RoundStartTimeOffset roundStartTimeOffset;
};
