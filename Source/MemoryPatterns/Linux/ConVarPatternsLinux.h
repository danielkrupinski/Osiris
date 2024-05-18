#pragma once

#include <GameClasses/OffsetTypes/ConVarOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct ConVarPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToConVarValueType offsetToConVarValueType() const noexcept
    {
        return patternFinders.tier0PatternFinder("66 41 89 46 ? 66"_pat).add(4).template readOffset<OffsetToConVarValueType>();
    }

    [[nodiscard]] OffsetToConVarValue offsetToConVarValue() const noexcept
    {
        return patternFinders.tier0PatternFinder("31 F6 48 8D 78 ? 48"_pat).add(5).template readOffset<OffsetToConVarValue>();
    }
};
