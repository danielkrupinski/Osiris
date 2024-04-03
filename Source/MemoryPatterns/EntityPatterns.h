#pragma once

#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <GameClasses/OffsetTypes/EntityOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct EntityPatterns {
    [[nodiscard]] OffsetToGameSceneNode offsetToGameSceneNode() const noexcept;
    [[nodiscard]] OffsetToHealth offsetToHealth() const noexcept;
    [[nodiscard]] OffsetToTeamNumber offsetToTeamNumber() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
