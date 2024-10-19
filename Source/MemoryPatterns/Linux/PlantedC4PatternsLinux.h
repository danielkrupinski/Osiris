#pragma once

#include <MemoryPatterns/PatternTypes/PlantedC4PatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlantedC4Patterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<PlantedC4sPointer, CodePattern{"? ? ? ? 8B 10 85 D2 0F 8F"}.abs()>()
            .template addPattern<BombSiteOffset, CodePattern{"83 BF ? ? ? ? 01 0F 94 C0 C3"}.add(2).read()>()
            .template addPattern<BombTickingOffset, CodePattern{"00 84 C0 74 ? 80 BB ? ? ? ? 00 74 ? 80"}.add(7).read()>()
            .template addPattern<BombBlowTimeOffset, CodePattern{"F3 0F 10 83 ? ? ? ? F3 0F 5C C1 0F 2F E0"}.add(4).read()>()
            .template addPattern<BombDefuserOffset, CodePattern{"E8 ? ? ? ? 8B 8B ? ? ? ? 83 F9 FF 0F 84 ? ? ? ? 48 8B 3D"}.add(7).read()>()
            .template addPattern<BombDefuseEndTimeOffset, CodePattern{"74 ? F3 0F 10 80 ? ? ? ? 84"}.add(6).read()>();
    }
};
