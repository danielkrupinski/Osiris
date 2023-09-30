#pragma once

#include <MemoryPatterns/GameRulesPatterns.h>

struct GameRulesImpl {
    [[nodiscard]] static const GameRulesImpl& instance() noexcept;

    RoundStartTimeOffset roundStartTimeOffset{ GameRulesPatterns::roundStartTimeOffset() };
};
