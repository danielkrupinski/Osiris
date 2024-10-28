#pragma once

#include <MemoryPatterns/PatternTypes/GlowSceneObjectPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GlowSceneObjectPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToGlowSceneObjectEntity, CodePattern{"48 89 B0 ? ? ? ? 48 8B ? ? 48 85"}.add(3).read()>()
            .template addPattern<OffsetToGlowSceneObjectAttachedSceneObject, CodePattern{"48 89 ? 48 89 ? ? ? ? ? 48 8B ? 48 8B"}.add(6).read()>();
    }
};
