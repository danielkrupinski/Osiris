#pragma once

#include <GameClasses/OffsetTypes/WeaponVDataOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct WeaponVDataPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToWeaponName offsetToWeaponName() const noexcept
    {
        return patternFinders.clientPatternFinder("74 ? 49 8B 85 ? ? ? ? 4C 8D 0D"_pat).add(5).template readOffset<OffsetToWeaponName>();
    }
};
