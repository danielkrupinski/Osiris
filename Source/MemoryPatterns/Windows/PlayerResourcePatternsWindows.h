#pragma once

#include <MemoryPatterns/PatternTypes/PlayerResourcePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerResourcePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PointerToPlayerResource, CodePattern{"80 8F ? ? ? ? ? 48 83 3D ? ? ? ?"}.add(10).abs(5)>()
            .template addPattern<GetBombsiteACenter, CodePattern{"54 24 ? E8 ? ? ? ? EB 0A"}.add(4).abs()>()
            .template addPattern<GetBombsiteBCenter, CodePattern{"EB 0A 48 8D 54 24 ? E8 ? ? ? ? F2"}.add(8).abs()>();
    }
};
