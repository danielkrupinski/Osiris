#pragma once

#include <MemoryPatterns/PlayerControllerPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToPlayerPawnHandle PlayerControllerPatterns::offsetToPlayerPawnHandle() const noexcept
{
    return clientPatternFinder("C1 48 85 C9 74 ? 8B 89 ? ? ? ?"_pat).add(8).readOffset<OffsetToPlayerPawnHandle>();
}

inline OffsetToPlayerColor PlayerControllerPatterns::offsetToPlayerColor() const noexcept
{
    return clientPatternFinder("E8 ? ? ? ? 84 C0 74 ? 41 8B ? ? ? ? ? EB"_pat).add(12).readOffset<OffsetToPlayerColor>();
}
