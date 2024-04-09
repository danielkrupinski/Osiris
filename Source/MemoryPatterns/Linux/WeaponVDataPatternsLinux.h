#pragma once

#include <MemoryPatterns/WeaponVDataPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToWeaponName WeaponVDataPatterns::offsetToWeaponName() const noexcept
{
    return clientPatternFinder("74 ? 48 8B ? ? ? ? ? 48 8D ? ? ? ? ? BE ? ? ? ? 8B 3D ? ? ? ? 48 8D"_pat).add(5).readOffset<OffsetToWeaponName>();
}
