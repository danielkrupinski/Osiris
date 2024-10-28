#pragma once

#include <MemoryPatterns/PatternTypes/WeaponServicesPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponServicesPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToActiveWeapon, CodePattern{"8B 57 ? 83 FA FF 74 ? 4C 8B 0D"}.add(2).read()>()
            .template addPattern<OffsetToWeapons, CodePattern{"47 ? 85 C0 0F 8E ? ? ? ? 55 48 89 E5 41 54"}.add(1).read()>();
    }
};
