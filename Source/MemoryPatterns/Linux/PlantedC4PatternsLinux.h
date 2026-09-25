#pragma once

#include <MemoryPatterns/PatternTypes/PlantedC4PatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlantedC4Patterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PlantedC4sPointer, CodePattern{"80 BF ? ? ? ? 00 0F 84 ? ? ? ? 48 8B 05 ? ? ? ? 48"}.add(16).abs()>()
            .template addPattern<BombSiteOffset, CodePattern{"? ? ? ? 41 B9 FF FF FF FF 66 0F EF C0 4C"}.read()>()
            .template addPattern<BombTickingOffset, CodePattern{"EC ? ? ? ? 80 BF ? ? ? ? 00 0F 84 ? ? ? ? 80 BF"}.add(7).read()>()
            .template addPattern<BombBlowTimeOffset, CodePattern{"? ? ? ? F3 0F 5C C1 0F 2F E0 0F 87 ? ? ? ? ? 8B"}.read()>()
            .template addPattern<BombDefuserOffset, CodePattern{"41 8B BE ? ? ? ? 45 8B"}.add(3).read()>()
            .template addPattern<BombDefuseEndTimeOffset, CodePattern{"74 ? F3 0F 10 80 ? ? ? ? 4C"}.add(6).read()>();
    }
};
