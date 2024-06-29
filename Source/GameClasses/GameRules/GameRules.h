#pragma once

#include "GameRulesCache.h"

struct GameRules {
    GameRulesCache& cache;

    [[nodiscard]] std::optional<float> roundStartTime() const noexcept
    {
        if (!cache.roundStartTime.has_value() && cache.gameRules) {
            if (const auto roundStartTime = GameRulesDeps::instance().roundStartTimeOffset.of(cache.gameRules); roundStartTime.get())
                cache.roundStartTime = *roundStartTime.get();
        }
        return cache.roundStartTime;
    }
};
