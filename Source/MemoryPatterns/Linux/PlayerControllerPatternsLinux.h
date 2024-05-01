#pragma once

#include <MemoryPatterns/PlayerControllerPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToPlayerPawnHandle PlayerControllerPatterns::offsetToPlayerPawnHandle() const noexcept
{
    return clientPatternFinder("F8 48 85 C0 74 ? 8B 88 ? ? ? ?"_pat).add(8).readOffset<OffsetToPlayerPawnHandle>();
}

inline OffsetToPlayerColor PlayerControllerPatterns::offsetToPlayerColor() const noexcept
{
    return clientPatternFinder("49 63 9C 24 ? ? ? ? 83 FB 04"_pat).add(4).readOffset<OffsetToPlayerColor>();
}
