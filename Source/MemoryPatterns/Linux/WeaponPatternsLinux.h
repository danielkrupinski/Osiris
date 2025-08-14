#pragma once

#include <MemoryPatterns/PatternTypes/WeaponPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToClipAmmo, CodePattern{"? ? ? ? 85 C0 7E ? 85 C0 ? 8B"}.read()>()
            .template addPattern<OffsetToWeaponSceneObjectUpdaterHandle, CodePattern{"E8 ? ? ? ? 48 89 83 ? ? ? ? 48 8D BB"}.add(8).read()>();
    }
};
