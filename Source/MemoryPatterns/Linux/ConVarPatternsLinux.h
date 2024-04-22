#pragma once

#include <MemoryPatterns/ConVarPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToConVarValueType ConVarPatterns::offsetToConVarValueType() const noexcept
{
    return tier0PatternFinder("66 41 89 46 ? 66"_pat).add(4).readOffset<OffsetToConVarValueType>();
}

inline OffsetToConVarValue ConVarPatterns::offsetToConVarValue() const noexcept
{
    return tier0PatternFinder("31 F6 48 8D 78 ? 48"_pat).add(5).readOffset<OffsetToConVarValue>();
}
