#pragma once

#include <MemoryPatterns/PatternTypes/GlowSceneObjectPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GlowSceneObjectPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToGlowSceneObjectEntity, CodePattern{"4C 89 B0 ? ? ? ? 49 8B 44 24"}.add(3).read()>()
            .template addPattern<OffsetToGlowSceneObjectAttachedSceneObject, CodePattern{"89 98 ? ? ? ? 49 8B ? 48"}.add(2).read()>();
    }
};
