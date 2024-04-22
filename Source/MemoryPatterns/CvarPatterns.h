#pragma once

#include <CS2/Classes/CCvar.h>
#include <GameClasses/OffsetTypes/CvarOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct CvarPatterns {
    [[nodiscard]] cs2::CCvar** cvar() const noexcept;
    [[nodiscard]] OffsetToConVarList offsetToConVarList() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
    const PatternFinder<PatternNotFoundLogger>& tier0PatternFinder;
};
