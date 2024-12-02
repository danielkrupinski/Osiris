#pragma once

#include <MemoryPatterns/PatternTypes/WeaponPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToClipAmmo, CodePattern{"83 ? ? ? ? ? 00 0F 85 ? ? ? ? 80 ? ? ? ? ? 00 74 ? 48"}.add(2).read()>()
            .template addPattern<OffsetToWeaponSceneObjectUpdaterHandle, CodePattern{"48 89 87 ? ? ? ? BA"}.add(3).read()>();
    }
};
