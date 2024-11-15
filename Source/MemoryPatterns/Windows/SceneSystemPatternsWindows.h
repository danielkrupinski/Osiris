#pragma once

#include <MemoryPatterns/PatternTypes/SceneSystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SceneSystemPatterns {
    [[nodiscard]] static consteval auto addSceneSystemPatterns(auto sceneSystemPatterns) noexcept
    {
        return sceneSystemPatterns
            .template addPattern<SceneSystemPointer, CodePattern{"48 8D 0D ? ? ? ? 48 8B 54 1A"}.add(3).abs()>()
            .template addPattern<DeleteSceneObjectFunctionPointer, CodePattern{"48 85 D2 0F 84 ? ? ? ? 48 89 54 24 ? 53"}>()
            .template addPattern<AllocateAttributeListFunctionPointer, CodePattern{"40 55 48 83 EC ? 48 83 BA"}>();
    }
};
