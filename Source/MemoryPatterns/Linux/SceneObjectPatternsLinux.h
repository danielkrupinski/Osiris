#pragma once

#include <MemoryPatterns/PatternTypes/SceneObjectPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SceneObjectPatterns {
    [[nodiscard]] static consteval auto addSceneSystemPatterns(auto sceneSystemPatterns) noexcept
    {
        return sceneSystemPatterns
            .template addPattern<OffsetToSceneObjectFlags, CodePattern{"F6 43 ? ? 0F 85 ? ? ? ? 48 8B 93"}.add(2).read()>()
            .template addPattern<OffsetToSceneObjectClass, CodePattern{"B6 46 ? 48 05"}.add(2).read()>()
            .template addPattern<OffsetToSceneObjectAttributes, CodePattern{"75 ? 49 83 BC 24 ? ? ? ? 00 74 ? 48 83 C4"}.add(6).read()>()
            .template addPattern<OffsetToSceneObjectRenderableFlags, CodePattern{"DE 48 89 93 ? ? ? ?"}.add(4).read()>();
    }
};
