#pragma once

#include <MemoryPatterns/CvarPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline cs2::CCvar** CvarPatterns::cvar() const noexcept
{
    return clientPatternFinder("48 83 EC ? ? 8B ? ? ? ? ? 48 8D 54 ? ? 4C"_pat).add(7).abs().as<cs2::CCvar**>();
}

inline OffsetToConVarList CvarPatterns::offsetToConVarList() const noexcept
{
    return tier0PatternFinder("74 ? 48 8B 41 ? 44 0F B7"_pat).add(5).readOffset<OffsetToConVarList>();
}
