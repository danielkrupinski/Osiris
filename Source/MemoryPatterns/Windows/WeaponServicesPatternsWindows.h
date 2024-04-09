#pragma once

#include <MemoryPatterns/WeaponServicesPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToActiveWeapon WeaponServicesPatterns::offsetToActiveWeapon() const noexcept
{
    return clientPatternFinder("? FF FF FF FF 48 85 D2 75 ? ? 8B"_pat).readOffset<OffsetToActiveWeapon>();
}
