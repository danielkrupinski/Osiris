#pragma once

#include <MemoryPatterns/PatternTypes/PlayerPawnPatternTypes.h>
#include <MemorySearch/CodePattern.h>

struct PlayerPawnPatterns {
    [[nodiscard]] static consteval auto addClientPatterns(auto clientPatterns) noexcept
    {
        return clientPatterns
            .template addPattern<OffsetToPlayerPawnImmunity, CodePattern{"34 01 EB 07 0F B6 87 ? ? ? ? 84 C0"}.add(7).read()>()
            .template addPattern<OffsetToMovementServices, CodePattern{"48 83 EC 28 48 8B 89 ? ? ? ? 48 85 C9 74 ? 48 8B 01 FF 90 20 01 00 00 48 85 C0 75 ? 8B 0D ? ? ? ? 65 48 8B 04 25 58 00 00 00 BA 68 00 00 00"}.add(7).read()>()
            .template addPattern<OffsetToWeaponServices, CodePattern{"48 8B 88 ? ? ? ? 48 8D 15 ? ? ? ? E8 ? ? ? ? 48"}.add(3).read()>()
            .template addPattern<OffsetToPlayerController, CodePattern{"8B 8B ? ? ? ? 44 88 7C"}.add(2).read()>()
            .template addPattern<OffsetToIsDefusing, CodePattern{"BF ? ? ? ? 00 75 ? 48 8B CF E8 ? ? ? ? 85"}.add(1).read()>()
            .template addPattern<OffsetToIsPickingUpHostage, CodePattern{"86 ? ? ? ? ? ? ? ? 80 BF ? ? ? ? 01"}.add(11).read()>()
            .template addPattern<OffsetToHostageServices, CodePattern{"0F 87 ? ? ? ? 48 8B 87 ? ? ? ? 8B 48 48"}.add(9).read()>()
            .template addPattern<OffsetToFlashBangEndTime, CodePattern{"10 87 ? ? ? ? 0F 2F ? ? 0F 86"}.add(2).read()>()
            .template addPattern<OffsetToPlayerPawnSceneObjectUpdaterHandle, CodePattern{"E8 ? ? ? ? 48 8B 8B ? ? ? ? 33 FF 48 85 C9 74 18 48 8B 93 ? ? ? ?"}.add(22).read()>()
            .template addPattern<OffsetToIsScoped, CodePattern{"88 B0 ? ? ? ? 0F 57 DB"}.add(2).read()>()
            .template addPattern<OffsetToEyeAngles, CodePattern{"C7 44 24 ? ? ? ? ? 4C 8D 45 ? 48 8D 15 ? ? ? ? C7 44 24 ? 90 00 00 00"}.add(4).read()>()
            .template addPattern<OffsetToStashGrenadeParameterWhen, CodePattern{"48 8B 47 08 48 8B 4B 38 F3 0F 10 80 ? ? ? ? F3 0F 11 81 ? ? ? ? 44 88 A1 ? ? ? ?"}.add(20).read()>()
            .template addPattern<OffsetToGrenadeParametersStashed, CodePattern{"48 8B 47 08 48 8B 4B 38 F3 0F 10 80 ? ? ? ? F3 0F 11 81 ? ? ? ? 44 88 A1 ? ? ? ?"}.add(27).read()>()
            .template addPattern<OffsetToStashedShootAngles, CodePattern{"48 8B D9 48 8B 89 ? ? ? ? 48 8B 01 FF 90 08 01 00 00 48 8B CB F2 0F 10 00 F2 0F 11 83 ? ? ? ? 8B 40 08 89 83 ? ? ? ?"}.add(30).read()>()
            .template addPattern<OffsetToStashedGrenadeThrowPosition, CodePattern{"F3 0F 58 07 F3 0F 10 4C 24 ? 48 8B CB F3 0F 58 4F 04 F3 0F 10 54 24 ? F3 0F 58 57 08 0F 14 C1 F2 0F 11 83 ? ? ? ? F3 0F 11 93 ? ? ? ?"}.add(36).read()>()
            .template addPattern<OffsetToStashedVelocity, CodePattern{"E8 ? ? ? ? F2 0F 10 00 F2 0F 11 83 ? ? ? ? 8B 40 08 89 83 ? ? ? ? 48 8B 5C 24 ? 48 83 C4 30"}.add(13).read()>();
    }
};
