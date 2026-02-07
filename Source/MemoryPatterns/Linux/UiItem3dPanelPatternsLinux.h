#pragma once

#include <MemoryPatterns/PatternTypes/UiItem3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct UiItem3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetItemItemIdFunction, CodePattern{"48 0F 44 D1 E9 ? ? ? ?"}.add(5).abs()>()
            .template addPattern<OffsetToItem3dPanelUnknownField, CodePattern{"41 39 44 24 ? 44"}.add(4).read()>()
            .template addPattern<OffsetToStartWeaponLookAt, CodePattern{"B8 01 00 00 00 66 89 87 ? ? ? ? C3"}.add(8).read()>()
            .template addPattern<OffsetToItem3dPanelProperties, CodePattern{"48 8D 8B ? ? ? ? 4C 89 F7"}.add(3).read()>();
    }
};
