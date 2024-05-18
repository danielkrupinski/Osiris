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
};
