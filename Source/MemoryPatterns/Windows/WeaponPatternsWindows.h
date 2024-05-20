#pragma once

#include <GameClasses/OffsetTypes/WeaponOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct WeaponPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToClipAmmo offsetToClipAmmo() const noexcept
    {
        return patternFinders.clientPatternFinder("83 ? ? ? ? ? 00 0F 85 ? ? ? ? 80 ? ? ? ? ? 00 74 ? 48"_pat).add(2).template readOffset<OffsetToClipAmmo>();
    }
};
