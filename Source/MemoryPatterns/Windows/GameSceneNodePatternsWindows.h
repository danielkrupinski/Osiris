#pragma once

#include <MemoryPatterns/GameSceneNodePatterns.h>
#include <MemorySearch/BytePatternLiteral.h>

inline OffsetToAbsOrigin GameSceneNodePatterns::offsetToAbsOrigin() const noexcept
{
    return OffsetToAbsOrigin{
        clientPatternFinder("F3 0F 11 97 ? ? ? ? 0F 28 C2"_pat).add(4).as<std::int32_t*>()
    };
}
