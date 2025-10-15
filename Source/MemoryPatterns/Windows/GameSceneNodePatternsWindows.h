#pragma once

#include <MemoryPatterns/PatternTypes/GameSceneNodePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GameSceneNodePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToAbsOrigin, CodePattern{"F3 0F 11 97 ? ? ? ? 0F 28 C2"}.add(4).read()>()
            .template addPattern<OffsetToGameSceneNodeOwner, CodePattern{"44 39 ? ? ? ? ? 75 ? 48 8B ? ? EB 03"}.add(12).read()>()
            .template addPattern<OffsetToChildGameSceneNode, CodePattern{"75 04 85 F6 74 ? ? 8B ? ? 48"}.add(9).read()>()
            .template addPattern<OffsetToNextSiblingGameSceneNode, CodePattern{"09 48 8B ? 48 8B ? FF 50 ? 48 8B ? ? 48 85 ? 75"}.add(13).read()>();
    }
};
