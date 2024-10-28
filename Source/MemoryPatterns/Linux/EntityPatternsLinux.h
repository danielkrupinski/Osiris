#pragma once

#include <MemoryPatterns/PatternTypes/EntityPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct EntityPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToGameSceneNode, CodePattern{"E8 ? ? ? ? 84 C0 75 ? 49 8B BC 24 ? ? ? ? 4C 89 EE"}.add(13).read()>()
            .template addPattern<OffsetToHealth, CodePattern{"C7 87 ? ? ? ? 00 00 00 00 48 8D 35"}.add(2).read()>()
            .template addPattern<OffsetToLifeState, CodePattern{"0F B6 97 ? ? ? ? 39 F2"}.add(3).read()>()
            .template addPattern<OffsetToTeamNumber, CodePattern{"41 0F B6 84 24 ? ? ? ? 3C 03"}.add(5).read()>()
            .template addPattern<OffsetToVData, CodePattern{"49 8B 84 24 ? ? ? ? 5A"}.add(4).read()>()
            .template addPattern<OffsetToRenderComponent, CodePattern{"49 8B BC 24 ? ? ? ? 48 85 FF 74 ? 8B 47"}.add(4).read()>()
            .template addPattern<OffsetToOwnerEntity, CodePattern{"8B BF ? ? ? ? 8B 8F ? ? ? ?"}.add(8).read()>();
    }
};
