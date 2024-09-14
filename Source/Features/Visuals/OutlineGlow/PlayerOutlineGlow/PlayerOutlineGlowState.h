#pragma once

#include "PlayerOutlineGlowColorType.h"

struct PlayerOutlineGlowState {
    bool enabled{true};
    bool showOnlyEnemies{true};
    PlayerOutlineGlowColorType colorType{PlayerOutlineGlowColorType::PlayerOrTeamColor};
};
