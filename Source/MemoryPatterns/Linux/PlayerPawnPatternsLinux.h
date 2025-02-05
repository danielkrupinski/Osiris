#pragma once

#include <MemoryPatterns/PatternTypes/PlayerPawnPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerPawnPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPlayerPawnImmunity, CodePattern{"75 ? 0F B6 83 ? ? ? ? 5B 41"}.add(5).read()>()
            .template addPattern<OffsetToWeaponServices, CodePattern{"48 8B BE ? ? ? ? 48 8D 35 ? ? ? ? E8 ? ? ? ? 48 89 C2"}.add(3).read()>()
            .template addPattern<OffsetToPlayerController, CodePattern{"77 ? 8B 8B ? ? ? ? 83"}.add(4).read()>()
            .template addPattern<OffsetToIsDefusing, CodePattern{"78 ? 00 75 ? 80 BB ? ? ? ?"}.add(7).read()>()
            .template addPattern<OffsetToIsPickingUpHostage, CodePattern{"E8 ? ? ? ? 41 80 BE ? ? ? ? 00 0F 85 ? ? ? ? ? 0F B6 ? ? ? ? ?"}.add(23).read()>()
            .template addPattern<OffsetToHostageServices, CodePattern{"C6 4C 89 C7 E8 ? ? ? ? 49 8B ? ? ? ? ?"}.add(12).read()>()
            .template addPattern<OffsetToFlashBangEndTime, CodePattern{"F3 0F 10 87 ? ? ? ? 4D"}.add(4).read()>()
            .template addPattern<OffsetToPlayerPawnSceneObjectUpdaterHandle, CodePattern{"89 83 ? ? ? ? 48 8B BB ? ? ? ? 48 8B"}.add(2).read()>();
    }
};
