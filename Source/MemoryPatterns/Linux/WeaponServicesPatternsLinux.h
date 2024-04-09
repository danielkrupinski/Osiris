#pragma once

#include <MemoryPatterns/WeaponServicesPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToActiveWeapon WeaponServicesPatterns::offsetToActiveWeapon() const noexcept
{
    return clientPatternFinder("8B 57 ? 83 FA FF 74 ? 4C 8B 0D"_pat).add(2).readOffset<OffsetToActiveWeapon>();
}
