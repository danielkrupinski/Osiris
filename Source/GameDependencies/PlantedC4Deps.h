#pragma once

#include <GameClasses/OffsetTypes/PlantedC4Offset.h>

struct PlantedC4Deps {
    PlantedC4Deps() = default;

    template <typename PlantedC4Patterns>
    explicit PlantedC4Deps(const PlantedC4Patterns& plantedC4Patterns) noexcept
        : plantedC4s{plantedC4Patterns.plantedC4s()}
        , bombSite{plantedC4Patterns.m_nBombSite()}
        , ticking{plantedC4Patterns.m_bBombTicking()}
        , blowTime{plantedC4Patterns.m_flC4Blow()}
        , defuser{plantedC4Patterns.m_hBombDefuser()}
        , defuseEndTime{plantedC4Patterns.m_flDefuseCountDown()}
    {
    }

    cs2::CUtlVector<cs2::CPlantedC4*>* plantedC4s;

    BombSiteOffset bombSite;
    BombTickingOffset ticking;
    BombBlowTimeOffset blowTime;
    BombDefuserOffset defuser;
    BombDefuseEndTimeOffset defuseEndTime;
};
