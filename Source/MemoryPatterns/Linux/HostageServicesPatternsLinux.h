#pragma once

#include <GameClasses/OffsetTypes/HostageServicesOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct HostageServicesPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToCarriedHostage offsetToCarriedHostage() const noexcept
    {
        return patternFinders.clientPatternFinder("44 8B 4E ? 89 C7"_pat).add(3).template readOffset<OffsetToCarriedHostage>();
    }
};
