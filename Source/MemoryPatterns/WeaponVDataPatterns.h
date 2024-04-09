#pragma once

#include <GameClasses/OffsetTypes/WeaponVDataOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct WeaponVDataPatterns {
    [[nodiscard]] OffsetToWeaponName offsetToWeaponName() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
