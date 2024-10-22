#pragma once

#include <MemoryPatterns/PatternTypes/SceneObjectPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SceneObjectPatterns {
    [[nodiscard]] static consteval auto addSceneSystemPatterns(auto sceneSystemPatterns) noexcept
    {
        return sceneSystemPatterns
            .template addPattern<OffsetToSceneObjectFlags, CodePattern{"06 ? ? ? ? ? 0F 85 ? ? ? ? 41 F6 44 24 ?"}.add(16).read()>()
            .template addPattern<OffsetToSceneObjectClass, CodePattern{"B6 46 ? 49 89 F4"}.add(2).read()>();
    }
};
