#pragma once

#include <MemoryPatterns/PatternTypes/GameSceneNodePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GameSceneNodePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToAbsOrigin, CodePattern{"F3 0F 11 97 ? ? ? ? 0F 28 C2"}.add(4).read()>()
            .template addPattern<OffsetToGameSceneNodeOwner, CodePattern{"44 39 B9 ? ? ? ? 75 ? 48 8B ? ?"}.add(12).read()>()
            .template addPattern<OffsetToChildGameSceneNode, CodePattern{"74 ? 49 8B 5E ? 48 85"}.add(5).read()>()
            .template addPattern<OffsetToNextSiblingGameSceneNode, CodePattern{"? 48 85 DB 75 ? 83 3D"}.read()>();
    }
};
