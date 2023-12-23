#pragma once

#include <CS2/Classes/CPlantedC4.h>
#include <GameClasses/OffsetTypes/PlantedC4Offset.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <MemorySearch/PatternFinder.h>

using BombSiteOffset = PlantedC4Offset<cs2::CPlantedC4::m_nBombSite, std::int32_t>;
using BombTickingOffset = PlantedC4Offset<cs2::CPlantedC4::m_bBombTicking, std::int32_t>;
using BombBlowTimeOffset = PlantedC4Offset<cs2::CPlantedC4::m_flC4Blow, std::int32_t>;
using BombDefuserOffset = PlantedC4Offset<cs2::CPlantedC4::m_hBombDefuser, std::int32_t>;
using BombDefuseEndTimeOffset = PlantedC4Offset<cs2::CPlantedC4::m_flDefuseCountDown, std::int32_t>;

struct PlantedC4Patterns {
    [[nodiscard]] BombSiteOffset m_nBombSite() const noexcept;
    [[nodiscard]] BombTickingOffset m_bBombTicking() const noexcept;
    [[nodiscard]] BombBlowTimeOffset m_flC4Blow() const noexcept;
    [[nodiscard]] BombDefuserOffset m_hBombDefuser() const noexcept;
    [[nodiscard]] BombDefuseEndTimeOffset m_flDefuseCountDown() const noexcept;

    const PatternFinder<PatternNotFoundLogger>& clientPatternFinder;
};
