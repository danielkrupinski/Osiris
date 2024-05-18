#pragma once

#include <GameClasses/OffsetTypes/GameSceneNodeOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct GameSceneNodePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToAbsOrigin offsetToAbsOrigin() const noexcept
    {
        return patternFinders.clientPatternFinder("48 89 83 ? ? ? ? 8B 83 ? ? ? ? 41 0F 2E 04 24"_pat).add(3).template readOffset<OffsetToAbsOrigin>();
    }
};
