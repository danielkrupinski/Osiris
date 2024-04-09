#pragma once

#include <MemoryPatterns/PlayerPawnPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToPlayerPawnImmunity PlayerPawnPatterns::offsetToPlayerPawnImmunity() const noexcept
{
    return clientPatternFinder("75 ? 0F B6 83 ? ? ? ? 5B"_pat).add(5).readOffset<OffsetToPlayerPawnImmunity>();
}

inline OffsetToWeaponServices PlayerPawnPatterns::offsetToWeaponServices() const noexcept
{
    return clientPatternFinder("48 8B BE ? ? ? ? 48 8D 35 ? ? ? ? E8 ? ? ? ? 48 89 C2"_pat).add(3).readOffset<OffsetToWeaponServices>();
}
