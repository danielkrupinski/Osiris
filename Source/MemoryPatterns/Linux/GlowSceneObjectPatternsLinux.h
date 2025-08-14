#pragma once

#include <MemoryPatterns/PatternTypes/GlowSceneObjectPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct GlowSceneObjectPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToGlowSceneObjectEntity, CodePattern{"4C 8B A6 ? ? ? ? 48 89"}.add(3).read()>()
            .template addPattern<OffsetToGlowSceneObjectAttachedSceneObject, CodePattern{"00 48 8B 07 FF 50 ? ? 89 ? ? 89 ? ? ? ? ? ? 8B"}.add(13).read()>();
    }
};
