#pragma once

#include <MemoryPatterns/EntityPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToGameSceneNode EntityPatterns::offsetToGameSceneNode() const noexcept
{
    return OffsetToGameSceneNode{
        clientPatternFinder("E8 ? ? ? ? 84 C0 75 ? 49 8B BC 24 ? ? ? ? 4C 89 EE"_pat).add(13).as<std::int32_t*>()
    };
}

inline OffsetToHealth EntityPatterns::offsetToHealth() const noexcept
{
    return OffsetToHealth{
        clientPatternFinder("8B 87 ? ? ? ? 85 C0 7E ? C7"_pat).add(2).as<std::int32_t*>()
    };
}

inline OffsetToTeamNumber EntityPatterns::offsetToTeamNumber() const noexcept
{
    return OffsetToTeamNumber{
        clientPatternFinder("41 0F B6 84 24 ? ? ? ? 3C 03"_pat).add(5).as<std::int32_t*>()
    };
}
