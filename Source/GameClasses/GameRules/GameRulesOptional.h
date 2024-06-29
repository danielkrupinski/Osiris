#pragma once

#include <cassert>

#include "GameRules.h"
#include "GameRulesCache.h"

struct GameRulesOptional {
    GameRulesOptional(cs2::C_CSGameRules** pointerToGameRules, GameRulesCache& gameRulesCache) noexcept
        : pointerToGameRules{pointerToGameRules}
        , gameRules{gameRulesCache}
    {
    }

    [[nodiscard]] operator bool() const noexcept
    {
        return gameRules.cache.gameRules || readGameRules();
    }

    [[nodiscard]] const GameRules* operator->() const noexcept
    {
        assert(static_cast<bool>(*this));
        return &gameRules;
    }

    [[nodiscard]] std::optional<float> roundStartTime() const noexcept
    {
        if (static_cast<bool>(*this))
            return gameRules.roundStartTime();
        return {};
    }

private:
    cs2::C_CSGameRules** pointerToGameRules;
    GameRules gameRules;

    [[nodiscard]] bool readGameRules() const noexcept
    {
        return pointerToGameRules && ((gameRules.cache.gameRules = *pointerToGameRules) != nullptr);
    }
};
