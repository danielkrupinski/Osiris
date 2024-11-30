#pragma once

#include <MemoryPatterns/PatternTypes/SceneObjectPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SceneObjectPatterns {
    [[nodiscard]] static consteval auto addSceneSystemPatterns(auto sceneSystemPatterns) noexcept
    {
        return sceneSystemPatterns
            .template addPattern<OffsetToSceneObjectFlags, CodePattern{"CC F6 83 ? ? ? ?"}.add(3).read()>()
            .template addPattern<OffsetToSceneObjectClass, CodePattern{"B6 82 ? ? ? ? 33"}.add(2).read()>()
            .template addPattern<OffsetToSceneObjectAttributes, CodePattern{"BA ? ? ? ? 00 48 8B EA 0F"}.add(1).read()>()
            .template addPattern<OffsetToSceneObjectRenderableFlags, CodePattern{"75 ? 48 8B 83 ? ? ? ? 48 C1"}.add(5).read()>();
    }
};
