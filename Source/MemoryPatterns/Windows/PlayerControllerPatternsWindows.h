#pragma once

#include <GameClasses/OffsetTypes/PlayerControllerOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PlayerControllerPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToPlayerPawnHandle offsetToPlayerPawnHandle() const noexcept
    {
        return patternFinders.clientPatternFinder("C1 48 85 C9 74 ? 8B ? ? ? ? ? 83"_pat).add(8).template readOffset<OffsetToPlayerPawnHandle>();
    }

    [[nodiscard]] OffsetToPlayerColor offsetToPlayerColor() const noexcept
    {
        return patternFinders.clientPatternFinder("E8 ? ? ? ? 84 C0 74 ? 41 8B ? ? ? ? ? EB"_pat).add(12).template readOffset<OffsetToPlayerColor>();
    }
};
