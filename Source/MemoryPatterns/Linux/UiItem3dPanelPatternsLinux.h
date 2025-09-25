#pragma once

#include <MemoryPatterns/PatternTypes/UiItem3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct UiItem3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetItemItemIdFunction, CodePattern{"5E 5D E9 ? ? ? ? 31 C0"}.add(3).abs()>()
            .template addPattern<OffsetToItem3dPanelUnknownField, CodePattern{"39 83 ? ? ? ? 0F 84 ? ? ? ? 48 8B B5"}.add(2).read()>();
    }
};
