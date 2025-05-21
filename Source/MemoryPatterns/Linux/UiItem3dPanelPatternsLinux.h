#pragma once

#include <MemoryPatterns/PatternTypes/UiItem3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct UiItem3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetItemItemIdFunction, CodePattern{"55 48 89 E5 41 57 41 56 49 89 D6 41 55 41 54 53 48 89 FB"}>()
            .template addPattern<OffsetToItem3dPanelUnknownField, CodePattern{"75 ? 8B 85 ? ? ? ? ? 83 ? ? ? ? 0F"}.add(10).read()>();
    }
};
