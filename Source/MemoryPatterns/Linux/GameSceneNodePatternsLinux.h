#pragma once

#include <MemoryPatterns/PatternTypes/GameSceneNodePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GameSceneNodePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToAbsOrigin, CodePattern{"43 ? F3 0F 10 83 ? ? ? ? 66"}.add(6).read()>()
            .template addPattern<OffsetToGameSceneNodeOwner, CodePattern{"4C 8B 6F ? ? F6 ? ? 0F"}.add(3).read()>()
            .template addPattern<OffsetToChildGameSceneNode, CodePattern{"? 48 85 DB 75 ? EB ? 0F B6"}.read()>()
            .template addPattern<OffsetToNextSiblingGameSceneNode, CodePattern{"50 ? 48 8B 5B ? 48 85 DB 0F"}.add(5).read()>();
    }
};
