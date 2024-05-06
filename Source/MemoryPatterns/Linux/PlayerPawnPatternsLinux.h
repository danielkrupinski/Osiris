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

inline OffsetToPlayerController PlayerPawnPatterns::offsetToPlayerController() const noexcept
{
    return clientPatternFinder("49 8D BD ? ? ? ? E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48"_pat).add(3).readOffset<OffsetToPlayerController>();
}

inline OffsetToIsDefusing PlayerPawnPatterns::offsetToIsDefusing() const noexcept
{
    return clientPatternFinder("78 ? 00 75 ? 80 BB ? ? ? ?"_pat).add(7).readOffset<OffsetToIsDefusing>();
}
