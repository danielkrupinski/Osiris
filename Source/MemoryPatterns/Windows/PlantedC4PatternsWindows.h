#pragma once

#include <MemoryPatterns/PatternTypes/PlantedC4PatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlantedC4Patterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PlantedC4sPointer, CodePattern{"0F ? ? ? ? ? 39 35 ? ? ? ? 7E ? 48 8B"}.add(8).abs()>()
            .template addPattern<BombSiteOffset, CodePattern{"83 B9 ? ? ? ? 01 0F 94 C0 C3"}.add(2).read()>()
            .template addPattern<BombTickingOffset, CodePattern{"74 ? 44 38 AE ? ? ? ? 74 ? 49"}.add(5).read()>()
            .template addPattern<BombBlowTimeOffset, CodePattern{"F3 0F 10 ? ? ? ? ? BB FF FF"}.add(4).read()>()
            .template addPattern<BombDefuserOffset, CodePattern{"E8 ? ? ? ? 8B ? ? ? ? ? 45 33 ? 4C 8B ? ? ? ? ? 83"}.add(7).read()>()
            .template addPattern<BombDefuseEndTimeOffset, CodePattern{"74 11 F3 0F 10 B7 ? ? ? ? 48"}.add(6).read()>();
    }
};
