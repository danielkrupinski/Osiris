#pragma once

#include <MemoryPatterns/PatternTypes/GameSceneNodePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GameSceneNodePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToGameSceneNodeOwner, CodePattern{"BD ? ? ? ? 00 0F 85 ? ? ? ? ? 8B ? ? 31 D2"}.add(15).read()>()
            .template addPattern<OffsetToChildGameSceneNode, CodePattern{"24 ? 48 85 DB 74 ? 48 89 DF E8 ? ? ? ? 48 8B"}.add(1).read()>()
            .template addPattern<OffsetToNextSiblingGameSceneNode, CodePattern{"? 48 85 DB 75 ? 49 8B 04 24 4C 89 E7 FF 50"}.read()>();
    }
};
