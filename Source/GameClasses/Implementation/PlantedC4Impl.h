#pragma once

#include <MemoryPatterns/PlantedC4Patterns.h>

struct PlantedC4Impl {
    [[nodiscard]] static const PlantedC4Impl& instance() noexcept;

    BombSiteOffset bombSite{ PlantedC4Patterns::m_nBombSite() };
    BombTickingOffset ticking{ PlantedC4Patterns::m_bBombTicking() };
    BombBlowTimeOffset blowTime{ PlantedC4Patterns::m_flC4Blow() };
    BombDefuserOffset defuser{ PlantedC4Patterns::m_hBombDefuser() };
    BombDefuseEndTimeOffset defuseEndTime{ PlantedC4Patterns::m_flDefuseCountDown() };
};
