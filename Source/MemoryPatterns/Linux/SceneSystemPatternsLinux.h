#pragma once

#include <MemoryPatterns/PatternTypes/SceneSystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SceneSystemPatterns {
    [[nodiscard]] static consteval auto addSceneSystemPatterns(auto sceneSystemPatterns) noexcept
    {
        return sceneSystemPatterns
            .template addPattern<SceneSystemPointer, CodePattern{"? ? ? ? F6 84 02 ? ? ? ? ? 75"}.abs()>()
            .template addPattern<DeleteSceneObjectFunctionPointer, CodePattern{"48 85 F6 0F 84 ? ? ? ? 55 48 89 E5 41 56 49"}>()
            .template addPattern<AllocateAttributeListFunctionPointer, CodePattern{"C3 48 8D 05 ? ? ? ? 48 8D 15 ? ? ? ?"}.add(11).abs()>();
    }
};
