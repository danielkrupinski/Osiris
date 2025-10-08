#pragma once

#include <MemoryPatterns/PatternTypes/Ui3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct Ui3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPortraitWorld, CodePattern{"? ? 89 ? ? 89 ? 8B B0"}.read()>()
            .template addPattern<OffsetTo3dPanelFov, CodePattern{"2E 83 ? ? ? ? C6 83 ? ? ? ? ? 7A"}.add(2).read()>();
    }
};
