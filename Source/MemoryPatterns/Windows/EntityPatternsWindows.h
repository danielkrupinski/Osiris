#pragma once

#include <MemoryPatterns/EntityPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToGameSceneNode EntityPatterns::offsetToGameSceneNode() const noexcept
{
    return clientPatternFinder("48 8B 8B ? ? ? ? 48 8B D7 48 8B 5C"_pat).add(3).readOffset<OffsetToGameSceneNode>();
}

inline OffsetToHealth EntityPatterns::offsetToHealth() const noexcept
{
    return clientPatternFinder("83 B9 ? ? ? ? 00 48 8B D9 7E ? C7"_pat).add(2).readOffset<OffsetToHealth>();
}

inline OffsetToLifeState EntityPatterns::offsetToLifeState() const noexcept
{
    return clientPatternFinder("0F B6 81 ? ? ? ? 3B C2"_pat).add(3).readOffset<OffsetToLifeState>();
}

inline OffsetToTeamNumber EntityPatterns::offsetToTeamNumber() const noexcept
{
    return clientPatternFinder("80 BB ? ? ? ? 03 75"_pat).add(2).readOffset<OffsetToTeamNumber>();
}

inline OffsetToVData EntityPatterns::offsetToVData() const noexcept
{
    return clientPatternFinder("49 8B 86 ? ? ? ? 48 85 C0 74 ? 48 8B 40"_pat).add(3).readOffset<OffsetToVData>();
}
