#pragma once

#include <MemoryPatterns/PatternTypes/WeaponServicesPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponServicesPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToActiveWeapon, CodePattern{"44 8B 4F ? 48 89 F8"}.add(3).read()>()
            .template addPattern<OffsetToWeapons, CodePattern{"8B 57 ? 48 8B 4F ? 8D"}.add(2).read()>();
    }
};
