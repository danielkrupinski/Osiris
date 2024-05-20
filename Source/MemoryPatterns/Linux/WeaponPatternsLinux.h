#pragma once

#include <GameClasses/OffsetTypes/WeaponOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct WeaponPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToClipAmmo offsetToClipAmmo() const noexcept
    {
        return patternFinders.clientPatternFinder("45 8B 85 ? ? ? ? 48 8D 15"_pat).add(3).template readOffset<OffsetToClipAmmo>();
    }
};
