#pragma once

#include <GameClasses/OffsetTypes/GameSceneNodeOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct GameSceneNodePatterns {
    [[nodiscard]] OffsetToAbsOrigin offsetToAbsOrigin() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
