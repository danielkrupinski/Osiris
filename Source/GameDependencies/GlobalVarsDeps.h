#pragma once

#include <CS2/Classes/GlobalVars.h>

struct GlobalVarsDeps {
    template <typename ClientPatterns>
    explicit GlobalVarsDeps(const ClientPatterns& clientPatterns) noexcept
        : globalVars{clientPatterns.globalVars()}
    {
    }

    cs2::GlobalVars** globalVars;
};
