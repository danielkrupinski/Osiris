#pragma once

#include <MemoryPatterns/PatternTypes/EntityPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct EntityPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToGameSceneNode, CodePattern{"C3 48 8B 8B ? ? ? ? 48 8B D7 48"}.add(4).read()>()
            .template addPattern<OffsetToHealth, CodePattern{"D9 ? ? C7 81 ? ? ? ? 00 00 00 00 48 8D 15"}.add(5).read()>()
            .template addPattern<OffsetToLifeState, CodePattern{"0F B6 81 ? ? ? ? 3B C2"}.add(3).read()>()
            .template addPattern<OffsetToTeamNumber, CodePattern{"80 BB ? ? ? ? 03 75"}.add(2).read()>()
            .template addPattern<OffsetToVData, CodePattern{"49 8B 86 ? ? ? ? 48 85 C0 74 ? 48 8B 40 ? 48 85 C0 74 ? 80"}.add(3).read()>()
            .template addPattern<OffsetToRenderComponent, CodePattern{"8B ? ? ? ? ? 48 85 C0 74 ? 39 ? ? 7E"}.add(2).read()>()
            .template addPattern<OffsetToOwnerEntity, CodePattern{"75 ? 8B 8E ? ? ? ? 3B CB"}.add(4).read()>();
    }
};
