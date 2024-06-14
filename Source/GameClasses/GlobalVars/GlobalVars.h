#pragma once

#include "GlobalVarsCache.h"

struct GlobalVars {
    GlobalVarsCache& cache;

    [[nodiscard]] std::optional<float> curtime() const noexcept
    {
        if (!cache.curtime.has_value() && cache.globalVars)
            cache.curtime = cache.globalVars->curtime;
        return cache.curtime;
    }
};
