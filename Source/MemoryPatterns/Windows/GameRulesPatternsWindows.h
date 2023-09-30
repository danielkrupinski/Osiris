#pragma once

#include <GlobalContext.h>
#include <MemoryPatterns/GameRulesPatterns.h>

inline RoundStartTimeOffset GameRulesPatterns::roundStartTimeOffset() noexcept
{
    return RoundStartTimeOffset{
        GlobalContext::instance().clientPatternFinder("F3 0F 5C C8 F3 0F 10 43 ? F3"_pat).add(8).as<std::int8_t*>()
    };
}
