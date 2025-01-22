#pragma once

#include <Utils/Optional.h>
#include <CS2/Classes/GlobalVars.h>

struct GlobalVars {
    [[nodiscard]] Optional<float> curtime() const noexcept
    {
        if (globalVars)
            return globalVars->curtime;
        return {};
    }

    cs2::GlobalVars* globalVars;
};
