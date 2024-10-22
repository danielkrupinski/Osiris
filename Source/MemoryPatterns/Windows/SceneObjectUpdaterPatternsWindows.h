#pragma once

#include <MemoryPatterns/PatternTypes/SceneObjectUpdaterPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct SceneObjectUpdaterPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToSceneObject, CodePattern{"C3 48 8B 42 ? C3"}.add(4).read()>();
    }
};
