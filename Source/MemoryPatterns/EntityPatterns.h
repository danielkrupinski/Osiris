#pragma once

#include <GameClasses/OffsetTypes/EntityOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct EntityPatterns {
    [[nodiscard]] OffsetToGameSceneNode offsetToGameSceneNode() const noexcept;
    [[nodiscard]] OffsetToHealth offsetToHealth() const noexcept;
    [[nodiscard]] OffsetToTeamNumber offsetToTeamNumber() const noexcept;
    [[nodiscard]] OffsetToVData offsetToVData() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
