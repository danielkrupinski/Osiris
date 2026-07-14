#pragma once

#include <MemoryPatterns/PatternTypes/PanelStylePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PanelStylePatterns2 {
    [[nodiscard]] static consteval auto addPanoramaPatterns(auto panoramaPatterns) noexcept
    {
        return panoramaPatterns
            .template addPattern<SetPanelStylePropertyFunctionPointer, CodePattern{"E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 45 ? EB ? 0F"}.add(1).abs()>()
            .template addPattern<FindStylePropertySymbolFunctionPointer, CodePattern{"48 89 5C 24 ? 57 48 83 EC 20 44 8B 05 ? ? ? ? 48 8B D9 65 48 8B 04 25"}>();
    }
};
