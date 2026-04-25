#pragma once

#include <MemoryPatterns/PatternTypes/SceneSystemPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SceneSystemPatterns {
    [[nodiscard]] static consteval auto addSceneSystemPatterns(auto sceneSystemPatterns) noexcept
    {
        return sceneSystemPatterns
            .template addPattern<SceneSystemPointer, CodePattern{"? ? ? ? F6 84 02 ? ? ? ? ? 75 ? 66"}.abs()>()
            .template addPattern<DeleteSceneObjectFunctionPointer, CodePattern{"55 48 8D 05 ? ? ? ? 48 89 E5 41 56 41 55 41 54 53 48 83"}>()
            .template addPattern<AllocateAttributeListFunctionPointer, CodePattern{"48 8B 3D ? ? ? ? 48 8D 15 ? ? ? ? 44"}.add(10).abs()>();
    }
};
