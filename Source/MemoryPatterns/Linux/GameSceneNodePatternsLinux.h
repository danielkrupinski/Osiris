#pragma once

#include <GameClasses/OffsetTypes/GameSceneNodeOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct GameSceneNodePatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToAbsOrigin offsetToAbsOrigin() const noexcept
    {
        return patternFinders.clientPatternFinder("43 ? F3 0F 10 83 ? ? ? ? 66"_pat).add(6).template readOffset<OffsetToAbsOrigin>();
    }
};
