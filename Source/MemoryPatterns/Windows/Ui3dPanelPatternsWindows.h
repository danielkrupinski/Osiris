#pragma once

#include <MemoryPatterns/PatternTypes/Ui3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct Ui3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPortraitWorld, CodePattern{"49 8B 47 ? 48 89 B4"}.add(3).read()>();
    }
};
