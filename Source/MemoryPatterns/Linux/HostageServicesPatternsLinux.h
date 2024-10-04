#pragma once

#include <GameClasses/OffsetTypes/HostageServicesOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct HostageServicesPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToCarriedHostage offsetToCarriedHostage() const noexcept
    {
        return patternFinders.clientPatternFinder("07 C7 47 ? FF FF FF FF C3"_pat).add(3).template readOffset<OffsetToCarriedHostage>();
    }
};
