#pragma once

#include <MemoryPatterns/PatternTypes/WeaponServicesPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponServicesPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToActiveWeapon, CodePattern{"09 D0 89 43 ? 48 85"}.add(4).read()>()
            .template addPattern<OffsetToWeapons, CodePattern{"8B 57 ? 48 8B 4F ? 8D"}.add(2).read()>();
    }
};
