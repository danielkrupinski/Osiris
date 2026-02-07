#pragma once

#include <MemoryPatterns/PatternTypes/EntityPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct EntityPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToGameSceneNode, CodePattern{"89 B4 24 ? ? ? ? 48 8B 98 ? ? ? ?"}.add(10).read()>()
            .template addPattern<OffsetToHealth, CodePattern{"D9 ? ? C7 81 ? ? ? ? 00 00 00 00 48 8D 15"}.add(5).read()>()
            .template addPattern<OffsetToLifeState, CodePattern{"0F B6 81 ? ? ? ? 3B C2"}.add(3).read()>()
            .template addPattern<OffsetToTeamNumber, CodePattern{"44 0F B6 89 ? ? ? ? 41 3B"}.add(4).read()>()
            .template addPattern<OffsetToVData, CodePattern{"89 ? ? ? ? 48 85 C9 74 ? 48 8B 49 ? 48 8D"}.add(1).read()>()
            .template addPattern<OffsetToRenderComponent, CodePattern{"8B ? ? ? ? ? 48 85 C0 74 ? 39 ? ? 7E"}.add(2).read()>()
            .template addPattern<OffsetToOwnerEntity, CodePattern{"8B 81 ? ? ? ? 4C 8B DA"}.add(2).read()>()
            .template addPattern<GetAbsOriginFunction, CodePattern{"F8 ? 75 ? E8 ? ? ? ? F3"}.add(5).abs()>();
    }
};
