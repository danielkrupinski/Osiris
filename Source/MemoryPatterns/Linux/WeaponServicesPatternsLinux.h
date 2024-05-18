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
};
