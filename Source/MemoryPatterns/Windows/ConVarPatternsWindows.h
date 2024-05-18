#pragma once

#include <GameClasses/OffsetTypes/ConVarOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct ConVarPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToConVarValueType offsetToConVarValueType() const noexcept
    {
        return patternFinders.tier0PatternFinder("66 89 4B ? 66 89 43 ? 48"_pat).add(3).template readOffset<OffsetToConVarValueType>();
    }

    [[nodiscard]] OffsetToConVarValue offsetToConVarValue() const noexcept
    {
        return patternFinders.tier0PatternFinder("48 8D 48 ? E8 ? ? ? ? 48 8D"_pat).add(3).template readOffset<OffsetToConVarValue>();
    }
};
