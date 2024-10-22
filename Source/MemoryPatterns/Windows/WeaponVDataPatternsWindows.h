#pragma once

#include <MemoryPatterns/PatternTypes/WeaponVDataPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponVDataPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToWeaponName, CodePattern{"74 ? 49 8B 85 ? ? ? ? 4C 8D 0D"}.add(5).read()>();
    }
};
