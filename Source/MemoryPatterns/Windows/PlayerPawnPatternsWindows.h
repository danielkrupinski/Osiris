#pragma once

#include <MemoryPatterns/PlayerPawnPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToPlayerPawnImmunity PlayerPawnPatterns::offsetToPlayerPawnImmunity() const noexcept
{
    return OffsetToPlayerPawnImmunity{
        clientPatternFinder("0F B6 83 ? ? ? ? 84 C0 75 ? 41"_pat).add(3).as<std::int32_t*>()
    };
}
