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
            .template addPattern<OffsetToRenderComponent, CodePattern{"48 8B 86 ? ? ? ? 33 DB 39 58 38 0F 8E ? ? ? ?"}.add(3).read()>()
            .template addPattern<OffsetToOwnerEntity, CodePattern{"8B 81 ? ? ? ? 4C 8B DA 4C 8B D1 41 83 F8 FF"}.add(2).read()>()
            .template addPattern<OffsetToViewOffset, CodePattern{"F3 0F 10 15 ? ? ? ? 48 81 C1 ? ? ? ? 33 D2 E9 ? ? ? ?"}.add(11).read()>()
            .template addPattern<OffsetToAbsVelocity, CodePattern{"48 8D 0D 5C E5 E9 01 C7 40 10 ? ? ? ? C6 00 03"}.add(10).read()>()
            .template addPattern<GetAbsOriginFunction, CodePattern{"F8 ? 75 ? E8 ? ? ? ? F3"}.add(5).abs()>();
    }
};
