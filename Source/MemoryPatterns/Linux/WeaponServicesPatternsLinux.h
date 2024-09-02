#pragma once

#include <GameClasses/OffsetTypes/WeaponServicesOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct WeaponServicesPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToActiveWeapon offsetToActiveWeapon() const noexcept
    {
        return patternFinders.clientPatternFinder("8B 57 ? 83 FA FF 74 ? 4C 8B 0D"_pat).add(2).template readOffset<OffsetToActiveWeapon>();
    }
    
    [[nodiscard]] OffsetToWeapons offsetToWeapons() const noexcept
    {
        return patternFinders.clientPatternFinder("47 ? 85 C0 0F 8E ? ? ? ? 55 48 89 E5 41 54"_pat).add(1).template readOffset<OffsetToWeapons>();
    }
};
