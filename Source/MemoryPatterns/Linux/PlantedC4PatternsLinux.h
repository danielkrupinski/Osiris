#pragma once

#include <MemoryPatterns/PatternTypes/PlantedC4PatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlantedC4Patterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PlantedC4sPointer, CodePattern{"80 BF ? ? ? ? 00 0F 84 ? ? ? ? 48 8B 05 ? ? ? ?"}.add(16).abs()>()
            .template addPattern<BombSiteOffset, CodePattern{"0F 86 ? ? ? ? 8B 83 ? ? ? ? 41 B9"}.add(8).read()>()
            .template addPattern<BombTickingOffset, CodePattern{"B6 00 84 C0 74 ? 80 BB ? ? ? ? 00 74 ? 80"}.add(8).read()>()
            .template addPattern<BombBlowTimeOffset, CodePattern{"F3 0F 10 83 ? ? ? ? F3 0F 5C C1 0F 2F E0"}.add(4).read()>()
            .template addPattern<BombDefuserOffset, CodePattern{"EC ? ? ? ? E8 ? ? ? ? 48 8D BB ? ? ? ? E8 ? ? ? ? 48"}.add(13).read()>()
            .template addPattern<BombDefuseEndTimeOffset, CodePattern{"74 ? F3 0F 10 80 ? ? ? ? 4C"}.add(6).read()>();
    }
};
