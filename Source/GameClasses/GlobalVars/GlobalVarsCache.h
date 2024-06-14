#pragma once

#include <optional>

#include <CS2/Classes/GlobalVars.h>

struct GlobalVarsCache {
    cs2::GlobalVars* globalVars{nullptr};
    std::optional<float> curtime;
};
