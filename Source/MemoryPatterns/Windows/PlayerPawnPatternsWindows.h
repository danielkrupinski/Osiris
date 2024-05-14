#pragma once

#include <MemoryPatterns/PlayerPawnPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToPlayerPawnImmunity PlayerPawnPatterns::offsetToPlayerPawnImmunity() const noexcept
{
    return clientPatternFinder("0F B6 83 ? ? ? ? 84 C0 75 ? 41"_pat).add(3).readOffset<OffsetToPlayerPawnImmunity>();
}

inline OffsetToWeaponServices PlayerPawnPatterns::offsetToWeaponServices() const noexcept
{
    return clientPatternFinder("48 8B 88 ? ? ? ? 48 8D 15 ? ? ? ? E8 ? ? ? ? 48"_pat).add(3).readOffset<OffsetToWeaponServices>();
}

inline OffsetToPlayerController PlayerPawnPatterns::offsetToPlayerController() const noexcept
{
    return clientPatternFinder("8B 89 ? ? ? ? 4C 89 60"_pat).add(2).readOffset<OffsetToPlayerController>();
}

inline OffsetToIsDefusing PlayerPawnPatterns::offsetToIsDefusing() const noexcept
{
    return clientPatternFinder("80 BF ? ? ? ? ? 0F 85 ? ? ? ? 80 BF ? ? ? ? ? 75"_pat).add(2).readOffset<OffsetToIsDefusing>();
}

inline OffsetToIsPickingUpHostage PlayerPawnPatterns::offsetToIsPickingUpHostage() const noexcept
{
    return clientPatternFinder("80 BF ? ? ? ? ? 0F 85 ? ? ? ? 80 BF ? ? ? ? ? 75"_pat).add(15).readOffset<OffsetToIsPickingUpHostage>();
}

inline OffsetToHostageServices PlayerPawnPatterns::offsetToHostageServices() const noexcept
{
    return clientPatternFinder("E8 ? ? ? ? 48 8B 87 ? ? ? ? 33 ? 4C"_pat).add(8).readOffset<OffsetToHostageServices>();
}
