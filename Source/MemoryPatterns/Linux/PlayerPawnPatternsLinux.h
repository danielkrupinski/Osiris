#pragma once

#include <MemoryPatterns/PlayerPawnPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToPlayerPawnImmunity PlayerPawnPatterns::offsetToPlayerPawnImmunity() const noexcept
{
    return clientPatternFinder("75 ? 0F B6 83 ? ? ? ? 5B"_pat).add(5).readOffset<OffsetToPlayerPawnImmunity>();
}
