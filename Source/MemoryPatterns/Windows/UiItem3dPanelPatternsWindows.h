#pragma once

#include <MemoryPatterns/PatternTypes/UiItem3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct UiItem3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetItemItemIdFunction, CodePattern{"4C 89 44 24 ? 48 89 4C 24 ? 55 56 57 41 54 48"}>()
            .template addPattern<OffsetToItem3dPanelUnknownField, CodePattern{"75 ? 39 B7 ? ? ? ? 75"}.add(4).read()>();
    }
};
