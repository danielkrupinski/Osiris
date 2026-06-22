#pragma once

#include <MemoryPatterns/PatternTypes/UiItem3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct UiItem3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetItemItemIdFunction, CodePattern{"70 ? 48 85 D2 48 0F 44 D1 E9 ? ? ? ?"}.add(10).abs()>()
            .template addPattern<OffsetToStartWeaponLookAt, CodePattern{"B8 01 00 00 00 66 89 87 ? ? ? ? C3"}.add(8).read()>();
    }
};
