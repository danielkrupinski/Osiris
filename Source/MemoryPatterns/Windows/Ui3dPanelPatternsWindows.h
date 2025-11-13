#pragma once

#include <MemoryPatterns/PatternTypes/Ui3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct Ui3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPortraitWorld, CodePattern{"56 ? 8B 92"}.add(1).read()>()
            .template addPattern<OffsetTo3dPanelFov, CodePattern{"0F 2E 83 ? ? ? ? C6"}.add(3).read()>()
            .template addPattern<OffsetTo3dPanelFovWeight, CodePattern{"0E F3 0F 11 83 ? ? ? ? 89 ? ? ? ? ?"}.add(11).read()>();
    }
};
