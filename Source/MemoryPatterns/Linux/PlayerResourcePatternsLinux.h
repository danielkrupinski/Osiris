#pragma once

#include <MemoryPatterns/PatternTypes/PlayerResourcePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerResourcePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PointerToPlayerResource, CodePattern{"C5 48 8D 05 ? ? ? ? 48 83 38 00 0F"}.add(4).abs()>()
            .template addPattern<GetBombsiteACenter, CodePattern{"E8 ? ? ? ? 66 0F D6 45 ? F3 0F 11 4D ? 4C 8D 6D"}.add(1).abs()>()
            .template addPattern<GetBombsiteBCenter, CodePattern{"01 4C 89 EF E8 ? ? ? ? 66"}.add(5).abs()>();
    }
};
