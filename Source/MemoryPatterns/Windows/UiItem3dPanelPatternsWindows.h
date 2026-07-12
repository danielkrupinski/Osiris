#pragma once

#include <MemoryPatterns/PatternTypes/UiItem3dPanelPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct UiItem3dPanelPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<SetItemItemIdFunction, CodePattern{"CF 48 8B D0 48 8B 5C 24 ? 48 83 C4 ? 5F E9 ? ? ? ?"}.add(15).abs()>()
            .template addPattern<OffsetToItem3dPanelUnknownField, CodePattern{"48 8B 85 E0 00 00 00 48 8D 15 ? ? ? ? 49 8D 4E 60 49 89 86 80 00 00 00 41 C6 86 94 00 00 00 01 FF 15 ? ? ? ? 41 C7 46 ? FF FF FF FF 48 8D 54 24 60 49 8B 46 60 49 8B 5D 10 48 85 C0 48 8B CB"}.add(42).read()>()
            .template addPattern<OffsetToStartWeaponLookAt, CodePattern{"81 ? ? ? ? 01 00 C3"}.add(1).read()>()
            .template addPattern<OffsetToItem3dPanelProperties, CodePattern{"8E ? ? ? ? 48 8D 45 ? C6"}.add(1).read()>();
    }
};
