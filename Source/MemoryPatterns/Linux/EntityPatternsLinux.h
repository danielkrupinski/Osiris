#pragma once

#include <MemoryPatterns/EntityPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToGameSceneNode EntityPatterns::offsetToGameSceneNode() const noexcept
{
    return clientPatternFinder("E8 ? ? ? ? 84 C0 75 ? 49 8B BC 24 ? ? ? ? 4C 89 EE"_pat).add(13).readOffset<OffsetToGameSceneNode>();
}

inline OffsetToHealth EntityPatterns::offsetToHealth() const noexcept
{
    return clientPatternFinder("8B 87 ? ? ? ? 85 C0 7E ? C7"_pat).add(2).readOffset<OffsetToHealth>();
}

inline OffsetToTeamNumber EntityPatterns::offsetToTeamNumber() const noexcept
{
    return clientPatternFinder("41 0F B6 84 24 ? ? ? ? 3C 03"_pat).add(5).readOffset<OffsetToTeamNumber>();
}

inline OffsetToVData EntityPatterns::offsetToVData() const noexcept
{
    return clientPatternFinder("49 8B 84 24 ? ? ? ? 5A"_pat).add(4).readOffset<OffsetToVData>();
}
