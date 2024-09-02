#pragma once

#include "PlayerOutlineGlowColorType.h"

struct PlayerOutlineGlowState {
    bool enabled{false};
    bool showOnlyEnemies{false};
    PlayerOutlineGlowColorType colorType{PlayerOutlineGlowColorType::PlayerOrTeamColor};
};
