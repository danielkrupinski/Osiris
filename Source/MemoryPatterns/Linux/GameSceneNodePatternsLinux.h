#pragma once

#include <MemoryPatterns/GameSceneNodePatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToAbsOrigin GameSceneNodePatterns::offsetToAbsOrigin() const noexcept
{
    return clientPatternFinder("48 89 83 ? ? ? ? 8B 83 ? ? ? ? 41 0F 2E 04 24"_pat).add(3).readOffset<OffsetToAbsOrigin>();
}
