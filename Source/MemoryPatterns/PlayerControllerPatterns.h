#pragma once

#include <GameClasses/OffsetTypes/PlayerControllerOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct PlayerControllerPatterns {
    [[nodiscard]] OffsetToPlayerPawnHandle offsetToPlayerPawnHandle() const noexcept;
    [[nodiscard]] OffsetToPlayerColor offsetToPlayerColor() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
