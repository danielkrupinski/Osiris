#pragma once

#include <MemoryPatterns/PatternTypes/UiItem3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct UiItem3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetItemItemIdFunction, CodePattern{"CF 48 8B D0 48 8B 5C 24 ? 48 83 C4 ? 5F E9 ? ? ? ?"}.add(15).abs()>()
            .template addPattern<OffsetToItem3dPanelUnknownField, CodePattern{"7E ? 75 28"}.add(1).read()>()
            .template addPattern<OffsetToStartWeaponLookAt, CodePattern{"81 ? ? ? ? 01 00 C3"}.add(1).read()>()
            .template addPattern<OffsetToItem3dPanelProperties, CodePattern{"8E ? ? ? ? 48 8D 45 ? C6"}.add(1).read()>();
    }
};
