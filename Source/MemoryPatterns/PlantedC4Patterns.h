#pragma once

#include <GameClasses/OffsetTypes/PlantedC4Offset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

struct PlantedC4Patterns {
    [[nodiscard]] BombSiteOffset m_nBombSite() const noexcept;
    [[nodiscard]] BombTickingOffset m_bBombTicking() const noexcept;
    [[nodiscard]] BombBlowTimeOffset m_flC4Blow() const noexcept;
    [[nodiscard]] BombDefuserOffset m_hBombDefuser() const noexcept;
    [[nodiscard]] BombDefuseEndTimeOffset m_flDefuseCountDown() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
