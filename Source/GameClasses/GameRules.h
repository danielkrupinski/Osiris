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

    [[nodiscard]] auto roundStartTime() const noexcept
    {
        return GameRulesDeps::instance().roundStartTimeOffset.of(gameRules).toOptional();
    }

    [[nodiscard]] auto roundRestartTime() const noexcept
    {
        return GameRulesDeps::instance().offsetToRoundRestartTime.of(gameRules).toOptional();
    }

    [[nodiscard]] bool hasScheduledRoundRestart() const noexcept
    {
        return roundRestartTime().greaterThan(0.0f).valueOr(false);
    }

    [[nodiscard]] auto timeToRoundRestart() const noexcept
    {
        return roundRestartTime() - hookContext.globalVars().curtime();
    }

    HookContext& hookContext;
    cs2::C_CSGameRules* gameRules;
};
