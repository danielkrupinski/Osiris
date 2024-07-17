#pragma once

#include <optional>

struct GlobalVars {
    [[nodiscard]] std::optional<float> curtime() const noexcept
    {
        if (globalVars)
            return globalVars->curtime;
        return {};
    }

    cs2::GlobalVars* globalVars;
};
