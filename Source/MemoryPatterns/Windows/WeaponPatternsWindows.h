#pragma once

#include <MemoryPatterns/PatternTypes/WeaponPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToClipAmmo, CodePattern{"75 07 8B 81 ? ? ? ? C3 8B 81 ? ? ? ?"}.add(11).read()>()
            .template addPattern<OffsetToWeaponMode, CodePattern{"80 B9 ? ? ? ? 00 74 ? F3 0F 10 05 ? ? ? ? C3 48 63 91 ? ? ? ? 48 8B 81 ? ? ? ? 85 D2 78 ? 48 83 FA 02"}.add(21).read()>()
            .template addPattern<OffsetToWeaponSceneObjectUpdaterHandle, CodePattern{"54 24 ? 48 8B ? E8 ? ? ? ? 48 89 ? ? ? ? ? B2"}.add(14).read()>()
            .template addPattern<PointerToGetInaccuracyFunction, CodePattern{"48 89 5C 24 ? 55 56 57 48 81 EC ? ? ? ? 44"}>();
    }
};
