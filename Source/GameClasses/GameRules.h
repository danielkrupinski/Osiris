#pragma once

#include <optional>

#include <GameDependencies/GameRulesDeps.h>

template <typename HookContext>
struct GameRules {
    GameRules(HookContext& hookContext, cs2::C_CSGameRules* gameRules) noexcept
        : hookContext{hookContext}
        , gameRules{gameRules}
    {
    }

    [[nodiscard]] std::optional<float> roundStartTime() const noexcept
    {
        if (gameRules && GameRulesDeps::instance().roundStartTimeOffset)
            return *GameRulesDeps::instance().roundStartTimeOffset.of(gameRules).get();
        return {};
    }

    [[nodiscard]] std::optional<float> roundRestartTime() const noexcept
    {
        if (gameRules && GameRulesDeps::instance().offsetToRoundRestartTime)
            return *GameRulesDeps::instance().offsetToRoundRestartTime.of(gameRules).get();
        return {};
    }

    [[nodiscard]] bool hasScheduledRoundRestart() const noexcept
    {
        return roundRestartTime() > 0.0f;
    }

    [[nodiscard]] std::optional<float> timeToRoundRestart() const noexcept
    {
        const auto _roundRestartTime = roundRestartTime();
        const auto _curtime = hookContext.globalVars().curtime();

        if (_roundRestartTime && _curtime)
            return *_roundRestartTime - *_curtime;
        return {};
    }

    HookContext& hookContext;
    cs2::C_CSGameRules* gameRules;
};
