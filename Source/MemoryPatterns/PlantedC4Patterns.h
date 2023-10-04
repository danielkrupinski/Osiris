#pragma once

#include <CS2/Classes/CPlantedC4.h>
#include <GameClasses/OffsetTypes/PlantedC4Offset.h>

using BombSiteOffset = PlantedC4Offset<cs2::CPlantedC4::m_nBombSite, std::int32_t>;
using BombTickingOffset = PlantedC4Offset<cs2::CPlantedC4::m_bBombTicking, std::int32_t>;
using BombBlowTimeOffset = PlantedC4Offset<cs2::CPlantedC4::m_flC4Blow, std::int32_t>;
using BombDefuserOffset = PlantedC4Offset<cs2::CPlantedC4::m_hBombDefuser, std::int32_t>;
using BombDefuseEndTimeOffset = PlantedC4Offset<cs2::CPlantedC4::m_flDefuseCountDown, std::int32_t>;

struct PlantedC4Patterns {
    [[nodiscard]] static BombSiteOffset m_nBombSite() noexcept;
    [[nodiscard]] static BombTickingOffset m_bBombTicking() noexcept;
    [[nodiscard]] static BombBlowTimeOffset m_flC4Blow() noexcept;
    [[nodiscard]] static BombDefuserOffset m_hBombDefuser() noexcept;
    [[nodiscard]] static BombDefuseEndTimeOffset m_flDefuseCountDown() noexcept;
};
