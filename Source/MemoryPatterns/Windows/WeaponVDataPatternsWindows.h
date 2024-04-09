#pragma once

#include <MemoryPatterns/WeaponVDataPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToWeaponName WeaponVDataPatterns::offsetToWeaponName() const noexcept
{
    return clientPatternFinder("74 ? 49 8B 85 ? ? ? ? 4C 8D 0D"_pat).add(5).readOffset<OffsetToWeaponName>();
}
