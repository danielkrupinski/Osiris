#pragma once

#include <MemoryPatterns/PatternTypes/WeaponVDataPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct WeaponVDataPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToWeaponName, CodePattern{"8B 80 ? ? ? ? 48 85 C0 48 0F 44"}.add(2).read()>()
            .template addPattern<OffsetToWeaponSpread, CodePattern{"93 ? ? ? ? 83 F8 01 0F 86 ? ? ? ? F3 0F 10 82 ? ? ? ?"}.add(18).read()>();
    }
};
