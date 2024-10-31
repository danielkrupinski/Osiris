#pragma once

#include <cstdint>

#include "Entities/C_BaseModelEntity.h"

namespace cs2
{

struct CPlantedC4 : C_BaseModelEntity {
    using m_nBombSite = int;
    using m_bBombTicking = bool;
    using m_hBombDefuser = std::uint32_t;
    using m_flC4Blow = float;
    using m_flDefuseCountDown = float;
};

}
