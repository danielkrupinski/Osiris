#pragma once

#include <MemoryPatterns/GameRulesPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline RoundStartTimeOffset GameRulesPatterns::roundStartTimeOffset() const noexcept
{
    return clientPatternFinder("F3 0F 5C C8 F3 0F 10 43 ? F3"_pat).add(8).readOffset<RoundStartTimeOffset>();
}
