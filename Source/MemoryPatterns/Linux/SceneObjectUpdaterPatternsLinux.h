#pragma once

#include <MemoryPatterns/PatternTypes/SceneObjectUpdaterPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SceneObjectUpdaterPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToSceneObject, CodePattern{"74 ? 48 8B 46 ? C3"}.add(5).read()>();
    }
};
