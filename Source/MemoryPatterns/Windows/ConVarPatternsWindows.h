#pragma once

#include <MemoryPatterns/ConVarPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToConVarValueType ConVarPatterns::offsetToConVarValueType() const noexcept
{
    return tier0PatternFinder("66 89 4B ? 66 89 43 ? 48"_pat).add(3).readOffset<OffsetToConVarValueType>();
}

inline OffsetToConVarValue ConVarPatterns::offsetToConVarValue() const noexcept
{
    return tier0PatternFinder("48 8D 48 ? E8 ? ? ? ? 48 8D"_pat).add(3).readOffset<OffsetToConVarValue>();
}
