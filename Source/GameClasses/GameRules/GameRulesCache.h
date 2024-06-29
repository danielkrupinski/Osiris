#pragma once

#include <optional>

namespace cs2 { struct C_CSGameRules; }

struct GameRulesCache {
    cs2::C_CSGameRules* gameRules{nullptr};
    std::optional<float> roundStartTime;
};
