#pragma once

#include <MemoryPatterns/PatternTypes/UiItem3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct UiItem3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetItemItemIdFunction, CodePattern{"55 48 89 E5 41 57 41 56 41 55 49 89 D5 41 54 49 89 FC 48 89 F7 53 48 81 EC ? ? ? ? E8"}>()
            .template addPattern<OffsetToItem3dPanelUnknownField, CodePattern{"85 ? ? ? ? 41 89 84 24 ? ? ? ? E9 ? ? ? ? 49"}.add(9).read()>();
    }
};
