#pragma once

#include <MemoryPatterns/PlayerControllerPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToPlayerPawnHandle PlayerControllerPatterns::offsetToPlayerPawnHandle() const noexcept
{
    return OffsetToPlayerPawnHandle{
        clientPatternFinder("F8 48 85 C0 74 ? 8B 88 ? ? ? ?"_pat).add(8).as<std::int32_t*>()
    };
}
