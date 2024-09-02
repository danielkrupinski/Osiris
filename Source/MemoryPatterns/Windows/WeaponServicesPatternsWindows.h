#pragma once

#include <GameClasses/OffsetTypes/WeaponServicesOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct WeaponServicesPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToActiveWeapon offsetToActiveWeapon() const noexcept
    {
        return patternFinders.clientPatternFinder("? FF FF FF FF 48 85 D2 75 ? ? 8B"_pat).template readOffset<OffsetToActiveWeapon>();
    }

    [[nodiscard]] OffsetToWeapons offsetToWeapons() const noexcept
    {
        return patternFinders.clientPatternFinder("DB 48 8B F9 39 59 ?"_pat).add(6).template readOffset<OffsetToWeapons>();
    }
};
