#pragma once

#include <GameClasses/OffsetTypes/HostageServicesOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct HostageServicesPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToCarriedHostage offsetToCarriedHostage() const noexcept
    {
        return patternFinders.clientPatternFinder("8B 48 ? 83 F9 FF 74 ? 4D 85 C0"_pat).add(2).template readOffset<OffsetToCarriedHostage>();
    }
};
