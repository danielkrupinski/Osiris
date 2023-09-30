#pragma once

#include <cstdint>

namespace cs2
{

struct CPlantedC4 {
    using m_nBombSite = int;
    using m_bBombTicking = bool;
    using m_hBombDefuser = std::uint32_t;
    using m_flC4Blow = float;
    using m_flDefuseCountDown = float;
};

}
