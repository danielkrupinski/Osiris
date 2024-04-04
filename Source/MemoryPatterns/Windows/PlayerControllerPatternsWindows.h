#pragma once

#include <MemoryPatterns/PlayerControllerPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToPlayerPawnHandle PlayerControllerPatterns::offsetToPlayerPawnHandle() const noexcept
{
    return clientPatternFinder("C1 48 85 C9 74 ? 8B 89 ? ? ? ?"_pat).add(8).readOffset<OffsetToPlayerPawnHandle>();
}
