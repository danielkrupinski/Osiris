#pragma once

#include <MemoryPatterns/PatternTypes/UiItem3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct UiItem3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetItemItemIdFunction, CodePattern{"55 48 89 E5 41 57 49 89 F7 41 56 41 55 41 54 53 48 89 FB 48 89 F7 48 81 EC ? ? ? ? 48 89 95"}>()
            .template addPattern<OffsetToItem3dPanelUnknownField, CodePattern{"39 83 ? ? ? ? 0F 84 ? ? ? ? 48 8B B5"}.add(2).read()>();
    }
};
