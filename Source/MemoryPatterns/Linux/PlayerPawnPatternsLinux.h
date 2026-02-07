#pragma once

#include <MemoryPatterns/PatternTypes/PlayerPawnPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerPawnPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPlayerPawnImmunity, CodePattern{"75 ? 0F B6 83 ? ? ? ? 5B 41"}.add(5).read()>()
            .template addPattern<OffsetToWeaponServices, CodePattern{"48 8B BE ? ? ? ? 48 8D 35 ? ? ? ? E8 ? ? ? ? 48 89 C2"}.add(3).read()>()
            .template addPattern<OffsetToPlayerController, CodePattern{"D0 89 87 ? ? ? ? 48 8B 07"}.add(3).read()>()
            .template addPattern<OffsetToIsDefusing, CodePattern{"? ? ? ? 00 75 ? 41 C6 ? ? ? ? ? 01 80"}.read()>()
            .template addPattern<OffsetToIsPickingUpHostage, CodePattern{"? ? ? ? 00 74 ? C6 86 ? ? ? ? 00 4C"}.read()>()
            .template addPattern<OffsetToHostageServices, CodePattern{"8B BB ? ? ? ? 31 F6 E8 ? ? ? ? ? 89 ? 4C"}.add(2).read()>()
            .template addPattern<OffsetToFlashBangEndTime, CodePattern{"41 0F 10 ? ? ? ? ? F3 0F 5C 05"}.add(4).read()>()
            .template addPattern<OffsetToPlayerPawnSceneObjectUpdaterHandle, CodePattern{"89 83 ? ? ? ? 48 8B BB ? ? ? ? 48 8B"}.add(2).read()>()
            .template addPattern<OffsetToIsScoped, CodePattern{"BB ? ? ? ? 00 F3 0F 11 45"}.add(1).read()>();
    }
};
