#pragma once

#include <cstddef>

#include <CS2/Classes/C_CSGameRules.h>
#include "Implementation/GameRulesImpl.h"

struct GameRules {
    explicit GameRules(cs2::C_CSGameRules* thisptr) noexcept
        : thisptr{ thisptr }
    {
    }

    [[nodiscard]] float getRoundStartTime() const noexcept
    {
        return GameRulesImpl::instance().roundStartTimeOffset.of(thisptr).valueOr(0.0f);
    }

private:
    cs2::C_CSGameRules* thisptr;
};
