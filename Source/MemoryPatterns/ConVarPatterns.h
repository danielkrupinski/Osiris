#pragma once

#include <GameClasses/OffsetTypes/ConVarOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct ConVarPatterns {
    [[nodiscard]] OffsetToConVarValueType offsetToConVarValueType() const noexcept;
    [[nodiscard]] OffsetToConVarValue offsetToConVarValue() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& tier0PatternFinder;
};
