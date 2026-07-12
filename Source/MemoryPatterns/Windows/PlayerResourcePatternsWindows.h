#pragma once

#include <MemoryPatterns/PatternTypes/PlayerResourcePatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerResourcePatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PointerToPlayerResource, CodePattern{"48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? C7 81 ? ? ? ? 00 00 00 00 48 8B 0D ? ? ? ? 0F 29"}.add(3).abs()>()
            .template addPattern<GetBombsiteACenter, CodePattern{"54 24 ? E8 ? ? ? ? EB 0A"}.add(4).abs()>()
            .template addPattern<GetBombsiteBCenter, CodePattern{"EB 0A 48 8D 54 24 ? E8 ? ? ? ? F2"}.add(8).abs()>();
    }
};
