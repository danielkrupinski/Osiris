#pragma once

#include <optional>

#include <GameDependencies/GameRulesDeps.h>

struct GameRules {
    [[nodiscard]] std::optional<float> roundStartTime() const noexcept
    {
        if (gameRules && GameRulesDeps::instance().roundStartTimeOffset)
            return *GameRulesDeps::instance().roundStartTimeOffset.of(gameRules).get();
        return {};
    }

    cs2::C_CSGameRules* gameRules;
};
