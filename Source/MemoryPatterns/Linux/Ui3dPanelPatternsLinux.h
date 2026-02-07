#pragma once

#include <MemoryPatterns/PatternTypes/Ui3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct Ui3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPortraitWorld, CodePattern{"84 ? ? ? ? 49 8B ? ? 8B B0 ? ? ? ? 48"}.add(8).read()>()
            .template addPattern<OffsetTo3dPanelFov, CodePattern{"2E 83 ? ? ? ? C6 83 ? ? ? ? ? 7A"}.add(2).read()>()
            .template addPattern<OffsetTo3dPanelFovWeight, CodePattern{"84 ? ? ? ? C7 83 ? ? ? ? 00 00 00 00 F3 0F 11"}.add(7).read()>();
    }
};
