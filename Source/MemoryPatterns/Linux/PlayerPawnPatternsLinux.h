#pragma once

#include <GameClasses/OffsetTypes/PlayerPawnOffset.h>
#include <MemorySearch/BytePatternLiteral.h>

template <typename PatternFinders>
struct PlayerPawnPatterns {
    const PatternFinders& patternFinders;

    [[nodiscard]] OffsetToPlayerPawnImmunity offsetToPlayerPawnImmunity() const noexcept
    {
        return patternFinders.clientPatternFinder("75 ? 0F B6 83 ? ? ? ? 5B"_pat).add(5).template readOffset<OffsetToPlayerPawnImmunity>();
    }

    [[nodiscard]] OffsetToWeaponServices offsetToWeaponServices() const noexcept
    {
        return patternFinders.clientPatternFinder("48 8B BE ? ? ? ? 48 8D 35 ? ? ? ? E8 ? ? ? ? 48 89 C2"_pat).add(3).template readOffset<OffsetToWeaponServices>();
    }

    [[nodiscard]] OffsetToPlayerController offsetToPlayerController() const noexcept
    {
        return patternFinders.clientPatternFinder("49 8D BD ? ? ? ? E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48"_pat).add(3).template readOffset<OffsetToPlayerController>();
    }

    [[nodiscard]] OffsetToIsDefusing offsetToIsDefusing() const noexcept
    {
        return patternFinders.clientPatternFinder("78 ? 00 75 ? 80 BB ? ? ? ?"_pat).add(7).template readOffset<OffsetToIsDefusing>();
    }

    [[nodiscard]] OffsetToIsPickingUpHostage offsetToIsPickingUpHostage() const noexcept
    {
        return patternFinders.clientPatternFinder("75 ? 41 C6 84 24 ? ? ? ? 01 49 8B 45"_pat).add(6).template readOffset<OffsetToIsPickingUpHostage>();
    }

    [[nodiscard]] OffsetToHostageServices offsetToHostageServices() const noexcept
    {
        return patternFinders.clientPatternFinder("E8 ? ? ? ? 49 8B B5 ? ? ? ? 48 85 F6 0F 84 ? ? ? ? 0F"_pat).add(8).template readOffset<OffsetToHostageServices>();
    }

    [[nodiscard]] OffsetToFlashBangEndTime offsetToFlashBangEndTime() const noexcept
    {
        return patternFinders.clientPatternFinder("F3 0F 10 83 ? ? ? ? 48 8B 18"_pat).add(4).template readOffset<OffsetToFlashBangEndTime>();
    }
};
