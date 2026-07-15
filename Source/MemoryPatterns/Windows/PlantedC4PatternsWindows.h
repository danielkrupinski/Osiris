#pragma once

#include <MemoryPatterns/PatternTypes/PlantedC4PatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlantedC4Patterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PlantedC4sPointer, CodePattern{"7E ? 49 8B 0E 48 8B 1D ? ? ? ? 48 8B D3 4C 8B 81 48 01 00 00"}.add(8).abs()>()
            .template addPattern<BombSiteOffset, CodePattern{"83 B9 ? ? ? ? 00 0F 94 C0 C3"}.add(2).read()>()
            .template addPattern<BombTickingOffset, CodePattern{"44 38 BB ? ? ? ? 0F 84 ? ? ? ? 48 8B 53 ? 48 8D 4D"}.add(3).read()>()
            .template addPattern<BombBlowTimeOffset, CodePattern{"BB ? ? ? ? 48 8B CF 48 8D 55"}.add(1).read()>()
            .template addPattern<BombDefuserOffset, CodePattern{"EC ? ? ? ? 48 8B ? E8 ? ? ? ? 8B ? ? ? ? ? 45 33 ? 4C"}.add(15).read()>()
            .template addPattern<BombDefuseEndTimeOffset, CodePattern{"8D 3D ? ? ? ? 48 8D 8B ? ? ? ?"}.add(9).read()>();
    }
};
