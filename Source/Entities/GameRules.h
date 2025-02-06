#pragma once

#include <CS2/Classes/C_CSGameRules.h>
#include <MemoryPatterns/PatternTypes/GameRulesPatternTypes.h>

template <typename HookContext>
class GameRules {
public:
    GameRules(HookContext& hookContext, cs2::C_CSGameRules* gameRules) noexcept
        : hookContext{hookContext}
        , gameRules{gameRules}
    {
    }

    [[nodiscard]] auto roundStartTime() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<RoundStartTimeOffset>().of(gameRules).toOptional();
    }

    [[nodiscard]] auto roundRestartTime() const noexcept
    {
        return hookContext.clientPatternSearchResults().template get<OffsetToRoundRestartTime>().of(gameRules).toOptional();
    }

    [[nodiscard]] bool hasScheduledRoundRestart() const noexcept
    {
        return roundRestartTime().greaterThan(0.0f).valueOr(false);
    }

    [[nodiscard]] auto timeToRoundRestart() const noexcept
    {
        return roundRestartTime() - hookContext.globalVars().curtime();
    }

private:
    HookContext& hookContext;
    cs2::C_CSGameRules* gameRules;
};
