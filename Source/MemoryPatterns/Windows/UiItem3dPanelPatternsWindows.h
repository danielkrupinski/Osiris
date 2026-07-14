#pragma once

#include <MemoryPatterns/PatternTypes/UiItem3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct UiItem3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetItemItemIdFunction, CodePattern{"CF 48 8B D0 48 8B 5C 24 ? 48 83 C4 ? 5F E9 ? ? ? ?"}.add(15).abs()>()
            .template addPattern<OffsetToItem3dPanelUnknownField, CodePattern{"85 C0 75 2A 41 39 76 ? 75 24 45 84 FF 0F 84"}.add(7).read()>()
            .template addPattern<OffsetToStartWeaponLookAt, CodePattern{"66 C7 81 ? ? ? ? 01 00 C3 CC CC CC CC CC CC"}.add(3).read()>()
            .template addPattern<OffsetToItem3dPanelProperties, CodePattern{"8E ? ? ? ? 48 8D 45 ? C6"}.add(1).read()>();
    }
};
