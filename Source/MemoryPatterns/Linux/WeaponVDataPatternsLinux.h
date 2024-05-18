#pragma once

#include <GameClasses/OffsetTypes/WeaponVDataOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct WeaponVDataPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToWeaponName offsetToWeaponName() const noexcept
    {
        return patternFinders.clientPatternFinder("74 ? 48 8B ? ? ? ? ? 48 8D ? ? ? ? ? BE ? ? ? ? 8B 3D ? ? ? ? 48 8D"_pat).add(5).template readOffset<OffsetToWeaponName>();
    }
};
