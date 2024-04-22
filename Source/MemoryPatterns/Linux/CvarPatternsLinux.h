#pragma once

#include <MemoryPatterns/CvarPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CCvar** CvarPatterns::cvar() const noexcept
{
    return clientPatternFinder("53 4C 8D 65 ? 48 83 EC ? 48 8D 05 ? ? ? ?"_pat).add(12).abs().as<cs2::CCvar**>();
}

inline OffsetToConVarList CvarPatterns::offsetToConVarList() const noexcept
{
    return tier0PatternFinder("74 ? 0F B7 F6 48 C1 E6 04 48 03 77 ?"_pat).add(12).readOffset<OffsetToConVarList>();
}
