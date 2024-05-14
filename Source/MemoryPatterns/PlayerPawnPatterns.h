#pragma once

#include <GameClasses/OffsetTypes/PlayerPawnOffset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct PlayerPawnPatterns {
    [[nodiscard]] OffsetToPlayerPawnImmunity offsetToPlayerPawnImmunity() const noexcept;
    [[nodiscard]] OffsetToWeaponServices offsetToWeaponServices() const noexcept;
    [[nodiscard]] OffsetToPlayerController offsetToPlayerController() const noexcept;
    [[nodiscard]] OffsetToIsDefusing offsetToIsDefusing() const noexcept;
    [[nodiscard]] OffsetToIsPickingUpHostage offsetToIsPickingUpHostage() const noexcept;
    [[nodiscard]] OffsetToHostageServices offsetToHostageServices() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
