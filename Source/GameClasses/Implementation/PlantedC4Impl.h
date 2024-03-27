#pragma once

#include <MemoryPatterns/PlantedC4Patterns.h>

struct PlantedC4Impl {
    explicit PlantedC4Impl(const PlantedC4Patterns& plantedC4Patterns) noexcept
        : bombSite{plantedC4Patterns.m_nBombSite()}
        , ticking{plantedC4Patterns.m_bBombTicking()}
        , blowTime{plantedC4Patterns.m_flC4Blow()}
        , defuser{plantedC4Patterns.m_hBombDefuser()}
        , defuseEndTime{plantedC4Patterns.m_flDefuseCountDown()}
    {
    }

    BombSiteOffset bombSite;
    BombTickingOffset ticking;
    BombBlowTimeOffset blowTime;
    BombDefuserOffset defuser;
    BombDefuseEndTimeOffset defuseEndTime;
};
