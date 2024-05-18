#pragma once

#include <GameClasses/OffsetTypes/PlayerControllerOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PlayerControllerPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToPlayerPawnHandle offsetToPlayerPawnHandle() const noexcept
    {
        return patternFinders.clientPatternFinder("F8 48 85 C0 74 ? 8B 88 ? ? ? ?"_pat).add(8).template readOffset<OffsetToPlayerPawnHandle>();
    }

    [[nodiscard]] OffsetToPlayerColor offsetToPlayerColor() const noexcept
    {
        return patternFinders.clientPatternFinder("49 63 9C 24 ? ? ? ? 83 FB 04"_pat).add(4).template readOffset<OffsetToPlayerColor>();
    }
};
