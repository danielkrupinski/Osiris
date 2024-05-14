#pragma once

#include <GameClasses/OffsetTypes/HostageServicesOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct HostageServicesPatterns {
    [[nodiscard]] OffsetToCarriedHostage offsetToCarriedHostage() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
