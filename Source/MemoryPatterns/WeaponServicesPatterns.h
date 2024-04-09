#pragma once

#include <GameClasses/OffsetTypes/WeaponServicesOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct WeaponServicesPatterns {
    [[nodiscard]] OffsetToActiveWeapon offsetToActiveWeapon() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
