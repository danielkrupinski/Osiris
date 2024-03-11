#pragma once

#include <MemoryPatterns/EntityPatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToGameSceneNode EntityPatterns::offsetToGameSceneNode() const noexcept
{
    return OffsetToGameSceneNode{
        clientPatternFinder("48 8B 8B ? ? ? ? 48 8B D7 48 8B 5C"_pat).add(3).as<std::int32_t*>()
    };
}

inline OffsetToHealth EntityPatterns::offsetToHealth() const noexcept
{
    return OffsetToHealth{
        clientPatternFinder("83 B9 ? ? ? ? 00 48 8B D9 7E ? C7"_pat).add(2).as<std::int32_t*>()
    };
}
