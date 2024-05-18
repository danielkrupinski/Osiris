#pragma once

#include <GameClasses/OffsetTypes/GameSceneNodeOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct GameSceneNodePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToAbsOrigin offsetToAbsOrigin() const noexcept
    {
        return patternFinders.clientPatternFinder("F3 0F 11 97 ? ? ? ? 0F 28 C2"_pat).add(4).template readOffset<OffsetToAbsOrigin>();
    }
};
