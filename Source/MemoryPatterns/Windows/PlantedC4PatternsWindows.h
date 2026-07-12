#pragma once

#include <MemoryPatterns/PatternTypes/PlantedC4PatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlantedC4Patterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PlantedC4Pointer, CodePattern{"48 8B 1D ? ? ? ? 33 FF 0F 29 74 24 20 48 8B F1"}.add(3).abs()>()
            .template addPattern<BombSiteOffset, CodePattern{"8B 8B ? ? ? ? 83 E9 01 74 0E 83 F9 01 75 2B"}.add(2).read()>()
            .template addPattern<BombTickingOffset, CodePattern{"80 BF ? ? ? ? 00 74 07 C6 87 A0 11 00 00 00 80 BF F4 11 00 00 01"}.add(2).read()>()
            .template addPattern<BombBlowTimeOffset, CodePattern{"BB ? ? ? ? 48 8B CF 48 8D 55"}.add(1).read()>()
            .template addPattern<BombDefuserOffset, CodePattern{"EC ? ? ? ? 48 8B ? E8 ? ? ? ? 8B ? ? ? ? ? 45 33 ? 4C"}.add(15).read()>()
            .template addPattern<BombDefuseEndTimeOffset, CodePattern{"8D 3D ? ? ? ? 48 8D 8B ? ? ? ?"}.add(9).read()>();
    }
};
