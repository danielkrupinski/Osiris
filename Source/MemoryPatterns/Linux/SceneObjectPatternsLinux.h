#pragma once

#include <MemoryPatterns/PatternTypes/SceneObjectPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SceneObjectPatterns {
    [[nodiscard]] static consteval auto addSceneSystemPatterns(auto sceneSystemPatterns) noexcept
    {
        return sceneSystemPatterns
            .template addPattern<OffsetToSceneObjectFlags, CodePattern{"84 06 ? ? ? ? ? 0F 85 ? ? ? ? F6 ? ? ? ? ?"}.add(15).read()>()
            .template addPattern<OffsetToSceneObjectClass, CodePattern{"0F B6 ? ? ? ? ? 48 05"}.add(3).read()>()
            .template addPattern<OffsetToSceneObjectAttributes, CodePattern{"89 9C 24 ? ? ? ? 48 83"}.add(3).read()>()
            .template addPattern<OffsetToSceneObjectRenderableFlags, CodePattern{"DE 48 89 53 ?"}.add(4).read()>();
    }
};
