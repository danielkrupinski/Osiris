#pragma once

#include <MemoryPatterns/PatternTypes/PlayerResourcePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerResourcePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PointerToPlayerResource, CodePattern{"C5 48 8D 05 ? ? ? ? 48 83 38 00 0F"}.add(4).abs()>()
            .template addPattern<OffsetToBombsiteACenter, CodePattern{"FF 48 C7 80 ? ? ? ? 00 00 00 00 0F 11 80 ? ? ? ?"}.add(15).read()>()
            .template addPattern<OffsetToBombsiteBCenter, CodePattern{"FF 48 C7 80 ? ? ? ? 00 00 00 00 0F 11 80"}.add(4).read()>();
    }
};
