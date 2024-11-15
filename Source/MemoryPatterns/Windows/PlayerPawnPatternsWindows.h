#pragma once

#include <MemoryPatterns/PatternTypes/PlayerPawnPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerPawnPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPlayerPawnImmunity, CodePattern{"0F B6 83 ? ? ? ? 84 C0 75 ? ? 80"}.add(3).read()>()
            .template addPattern<OffsetToWeaponServices, CodePattern{"48 8B 88 ? ? ? ? 48 8D 15 ? ? ? ? E8 ? ? ? ? 48"}.add(3).read()>()
            .template addPattern<OffsetToPlayerController, CodePattern{"8B 89 ? ? ? ? 4C 89 60"}.add(2).read()>()
            .template addPattern<OffsetToIsDefusing, CodePattern{"80 BF ? ? ? ? ? 0F 85 ? ? ? ? 80 BF ? ? ? ? ? 75"}.add(2).read()>()
            .template addPattern<OffsetToIsPickingUpHostage, CodePattern{"80 BF ? ? ? ? ? 0F 85 ? ? ? ? 80 BF ? ? ? ? ? 75"}.add(15).read()>()
            .template addPattern<OffsetToHostageServices, CodePattern{"E8 ? ? ? ? 48 8B 87 ? ? ? ? 33 ? 4C"}.add(8).read()>()
            .template addPattern<OffsetToFlashBangEndTime, CodePattern{"? ? ? ? 0F 2F 43 ? 0F 86 ? ? ? ? 48"}.read()>()
            .template addPattern<OffsetToPlayerPawnSceneObjectUpdaterHandle, CodePattern{"89 83 ? ? ? ? 48 8B 8B ? ? ? ? 48 8B 01"}.add(2).read()>();
    }
};
