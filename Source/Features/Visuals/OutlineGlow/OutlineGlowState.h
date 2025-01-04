#pragma once

#include "PlayerOutlineGlow/PlayerOutlineGlowColorType.h"

struct OutlineGlowState {
    bool masterSwitch{false};
    bool enabledForWeapons{true};
    bool enabledForTickingBomb{true};
    bool enabledForDroppedBomb{true};
    bool enabledForHostages{true};
    bool enabledForDefuseKits{true};
    bool enabledForGrenadeProjectiles{true};
    bool enabledForPlayers{true};
    bool showOnlyEnemies{true};
    PlayerOutlineGlowColorType playerGlowColorType{PlayerOutlineGlowColorType::PlayerOrTeamColor};
};
