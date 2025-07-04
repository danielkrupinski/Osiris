#pragma once

#include <MemoryPatterns/PatternTypes/WeaponVDataPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponVDataPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToWeaponName, CodePattern{"49 89 86 ? ? ? ? E8 ? ? ? ? 49"}.add(3).read()>();
    }
};
