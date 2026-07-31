#pragma once

#include <MemoryPatterns/PatternTypes/PlantedC4PatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlantedC4Patterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PlantedC4sPointer, CodePattern{"49 8B 0E 48 8B 1D ? ? ? ?"}.add(6).abs()>()
            .template addPattern<BombSiteOffset, CodePattern{"41 0F 2F C2 0F 86 ? ? ? ? 8B ? ? ? ? ?"}.add(12).read()>()
            .template addPattern<BombTickingOffset, CodePattern{"44 38 ? ? ? ? ? 74 ? 49 8B ? E8 ? ? ? ? 83"}.add(3).read()>()
            .template addPattern<BombBlowTimeOffset, CodePattern{"F3 0F 10 87 ? ? ? ? F3 0F 11 06"}.add(4).read()>()
            .template addPattern<BombDefuserOffset, CodePattern{"EC ? ? ? ? 48 8B ? E8 ? ? ? ? 8B ? ? ? ? ? 45 33 ? 4C"}.add(15).read()>()
            .template addPattern<BombDefuseEndTimeOffset, CodePattern{"8D 3D ? ? ? ? 48 8D 8B ? ? ? ?"}.add(9).read()>();
    }
};
