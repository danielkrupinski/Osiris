#pragma once

#include <MemoryPatterns/PatternTypes/PlantedC4PatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlantedC4Patterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PlantedC4sPointer, CodePattern{"0F ? ? ? ? ? 39 ? ? ? ? ? 7E ? 48 8B 0D"}.add(8).abs()>()
            .template addPattern<BombSiteOffset, CodePattern{"83 B9 ? ? ? ? 01 0F 94 C0 C3"}.add(2).read()>()
            .template addPattern<BombTickingOffset, CodePattern{"74 ? 44 38 AE ? ? ? ? 74 ? 49"}.add(5).read()>()
            .template addPattern<BombBlowTimeOffset, CodePattern{"BB ? ? ? ? 48 8B CF 48 8D 55"}.add(1).read()>()
            .template addPattern<BombDefuserOffset, CodePattern{"EC ? ? ? ? 48 8B ? E8 ? ? ? ? 8B ? ? ? ? ? 45 33 ? 4C"}.add(15).read()>()
            .template addPattern<BombDefuseEndTimeOffset, CodePattern{"8D 3D ? ? ? ? 48 8D 8B ? ? ? ?"}.add(9).read()>();
    }
};
