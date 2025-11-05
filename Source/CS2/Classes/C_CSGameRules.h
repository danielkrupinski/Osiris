#pragma once

#include <CS2/Constants/RoundWinStatus.h>

namespace cs2
{

struct C_CSGameRules {
    using m_fRoundStartTime = float;
    using m_flRestartRoundTime = float;
    using m_iRoundTime = int;
    using m_iRoundWinStatus = RoundWinStatus;
};

}
