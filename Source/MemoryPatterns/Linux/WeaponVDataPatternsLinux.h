#pragma once

#include <MemoryPatterns/PatternTypes/WeaponVDataPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponVDataPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToWeaponName, CodePattern{"74 ? 48 8B ? ? ? ? ? 48 8D ? ? ? ? ? BE ? ? ? ? 8B 3D ? ? ? ? 48 8D"}.add(5).read()>();
    }
};
