#pragma once

#include <MemoryPatterns/PatternTypes/WeaponServicesPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponServicesPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToActiveWeapon, CodePattern{"C0 0F 85 ? ? ? ? 8B 4F ? 83"}.add(9).read()>()
            .template addPattern<OffsetToWeapons, CodePattern{"? 4C 8B 49 ? C1"}.read()>();
    }
};
