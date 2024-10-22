#pragma once

#include <MemoryPatterns/PatternTypes/WeaponServicesPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponServicesPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToActiveWeapon, CodePattern{"? FF FF FF FF 48 85 D2 75 ? ? 8B"}.read()>()
            .template addPattern<OffsetToWeapons, CodePattern{"DB 48 8B F9 39 59 ?"}.add(6).read()>();
    }
};
