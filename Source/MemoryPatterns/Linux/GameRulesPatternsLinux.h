#pragma once

#include <MemoryPatterns/GameRulesPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline RoundStartTimeOffset GameRulesPatterns::roundStartTimeOffset() const noexcept
{
    return RoundStartTimeOffset{
        clientPatternFinder("F3 0F 10 43 ? F3 0F 5C C1 66 0F EF C9"_pat).add(4).as<std::int8_t*>()
    };
}
