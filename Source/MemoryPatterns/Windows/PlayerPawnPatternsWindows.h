#pragma once

#include <GameClasses/OffsetTypes/PlayerPawnOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PlayerPawnPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToPlayerPawnImmunity offsetToPlayerPawnImmunity() const noexcept
    {
        return patternFinders.clientPatternFinder("0F B6 83 ? ? ? ? 84 C0 75 ? ? 80"_pat).add(3).template readOffset<OffsetToPlayerPawnImmunity>();
    }

    [[nodiscard]] OffsetToWeaponServices offsetToWeaponServices() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8B 88 ? ? ? ? 48 8D 15 ? ? ? ? E8 ? ? ? ? 48"_pat).add(3).template readOffset<OffsetToWeaponServices>();
    }

    [[nodiscard]] OffsetToPlayerController offsetToPlayerController() const noexcept
    {
        return patternFinders.clientPatternFinder("8B 89 ? ? ? ? 4C 89 60"_pat).add(2).template readOffset<OffsetToPlayerController>();
    }

    [[nodiscard]] OffsetToIsDefusing offsetToIsDefusing() const noexcept
    {
        return patternFinders.clientPatternFinder("80 BF ? ? ? ? ? 0F 85 ? ? ? ? 80 BF ? ? ? ? ? 75"_pat).add(2).template readOffset<OffsetToIsDefusing>();
    }

    [[nodiscard]] OffsetToIsPickingUpHostage offsetToIsPickingUpHostage() const noexcept
    {
        return patternFinders.clientPatternFinder("80 BF ? ? ? ? ? 0F 85 ? ? ? ? 80 BF ? ? ? ? ? 75"_pat).add(15).template readOffset<OffsetToIsPickingUpHostage>();
    }

    [[nodiscard]] OffsetToHostageServices offsetToHostageServices() const noexcept
    {
        return patternFinders.clientPatternFinder("E8 ? ? ? ? 48 8B 87 ? ? ? ? 33 ? 4C"_pat).add(8).template readOffset<OffsetToHostageServices>();
    }

    [[nodiscard]] OffsetToFlashBangEndTime offsetToFlashBangEndTime() const noexcept
    {
        return patternFinders.clientPatternFinder("? ? ? ? 0F 2F 43 ? 0F 86 ? ? ? ? 48"_pat).template readOffset<OffsetToFlashBangEndTime>();
    }
};
