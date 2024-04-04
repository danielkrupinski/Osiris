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

inline OffsetToTeamNumber EntityPatterns::offsetToTeamNumber() const noexcept
{
    return clientPatternFinder("80 BB ? ? ? ? 03 75"_pat).add(2).readOffset<OffsetToTeamNumber>();
}
