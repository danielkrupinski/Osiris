#pragma once

#include <CS2/Classes/GlobalVars.h>
#include <Utils/Optional.h>

struct GlobalVars {
    [[nodiscard]] Optional<float> curtime() const noexcept
    {
        if (globalVars)
            return globalVars->curtime;
        return {};
    }

    [[nodiscard]] Optional<float> frametime() const noexcept
    {
        if (globalVars)
            return globalVars->frametime;
        return {};
    }

    cs2::GlobalVars* globalVars;
};
