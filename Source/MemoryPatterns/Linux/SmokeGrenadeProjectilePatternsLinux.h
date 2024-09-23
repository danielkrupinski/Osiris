#pragma once

#include <GameClasses/OffsetTypes/SmokeGrenadeProjectileOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct SmokeGrenadeProjectilePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToDidSmokeEffect offsetToDidSmokeEffect() const noexcept
    {
        return patternFinders.clientPatternFinder("85 F6 75 ? 80 BF ? ? ? ?"_pat).add(6).template readOffset<OffsetToDidSmokeEffect>();
    }
};
