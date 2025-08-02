#pragma once

#include <MemoryPatterns/PatternTypes/WeaponPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToClipAmmo, CodePattern{"74 ? 83 ? ? ? ? ? 00 7C ? 8B"}.add(4).read()>()
            .template addPattern<OffsetToWeaponSceneObjectUpdaterHandle, CodePattern{"24 ? 48 8B CB E8 ? ? ? ? 48 89 87 ? ? ? ?"}.add(13).read()>();
    }
};
