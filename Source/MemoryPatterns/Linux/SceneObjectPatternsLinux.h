#pragma once

#include <MemoryPatterns/PatternTypes/SceneObjectPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SceneObjectPatterns {
    [[nodiscard]] static consteval auto addSceneSystemPatterns(auto sceneSystemPatterns) noexcept
    {
        return sceneSystemPatterns
            .template addPattern<OffsetToSceneObjectFlags, CodePattern{"85 ? ? ? ? 41 F6 84 24 ? ? ? ? 20 0F 85"}.add(9).read()>()
            .template addPattern<OffsetToSceneObjectClass, CodePattern{"0F B6 ? ? ? ? ? 48 05"}.add(3).read()>()
            .template addPattern<OffsetToSceneObjectAttributes, CodePattern{"74 ? 4D 8B A6 ? ? ? ? 4D"}.add(5).read()>()
            .template addPattern<OffsetToSceneObjectRenderableFlags, CodePattern{"49 8B 54 24 ? 48 89 D0"}.add(4).read()>();
    }
};
