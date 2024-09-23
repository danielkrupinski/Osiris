#pragma once

#include <GameClasses/OffsetTypes/SmokeGrenadeProjectileOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct SmokeGrenadeProjectilePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToDidSmokeEffect offsetToDidSmokeEffect() const noexcept
    {
        return patternFinders.clientPatternFinder("85 D2 75 ? 38 91 ? ? ? ?"_pat).add(6).template readOffset<OffsetToDidSmokeEffect>();
    }
};
