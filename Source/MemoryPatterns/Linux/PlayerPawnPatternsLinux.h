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

inline OffsetToIsPickingUpHostage PlayerPawnPatterns::offsetToIsPickingUpHostage() const noexcept
{
    return clientPatternFinder("75 ? 41 C6 84 24 ? ? ? ? 01 49 8B 45"_pat).add(6).readOffset<OffsetToIsPickingUpHostage>();
}

inline OffsetToHostageServices PlayerPawnPatterns::offsetToHostageServices() const noexcept
{
    return clientPatternFinder("E8 ? ? ? ? 49 8B B5 ? ? ? ? 48 85 F6 0F 84 ? ? ? ? 0F"_pat).add(8).readOffset<OffsetToHostageServices>();
}

inline OffsetToFlashBangEndTime PlayerPawnPatterns::offsetToFlashBangEndTime() const noexcept
{
    return clientPatternFinder("F3 0F 10 83 ? ? ? ? 48 8B 18"_pat).add(4).readOffset<OffsetToFlashBangEndTime>();
}
