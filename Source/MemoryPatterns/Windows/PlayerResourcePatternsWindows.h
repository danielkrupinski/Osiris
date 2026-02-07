#pragma once

#include <MemoryPatterns/PatternTypes/PlayerResourcePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerResourcePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PointerToPlayerResource, CodePattern{"80 8F ? ? ? ? ? 48 83 3D ? ? ? ?"}.add(10).abs(5)>()
            .template addPattern<OffsetToBombsiteACenter, CodePattern{"03 48 8D 8B ? ? ? ? 0F"}.add(4).read()>()
            .template addPattern<OffsetToBombsiteBCenter, CodePattern{"11 83 ? ? ? ? E8 ? ? ? ? 48 8D 8B ? ? ? ?"}.add(14).read()>();
    }
};
