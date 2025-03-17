#pragma once

#include <Config/ConfigVariable.h>
#include "PlayerOutlineGlow/PlayerOutlineGlowColorType.h"

CONFIG_VARIABLE(OutlineGlowEnabled, bool, false);
CONFIG_VARIABLE(DefuseKitOutlineGlowEnabled, bool, true);
CONFIG_VARIABLE(DroppedBombOutlineGlowEnabled, bool, true);
CONFIG_VARIABLE(GrenadeProjectileOutlineGlowEnabled, bool, true);
CONFIG_VARIABLE(HostageOutlineGlowEnabled, bool, true);
CONFIG_VARIABLE(PlayerOutlineGlowEnabled, bool, true);
CONFIG_VARIABLE(PlayerOutlineGlowOnlyEnemies, bool, true);
CONFIG_VARIABLE(PlayerOutlineGlowColorMode, PlayerOutlineGlowColorType, PlayerOutlineGlowColorType::PlayerOrTeamColor);
CONFIG_VARIABLE(TickingBombOutlineGlowEnabled, bool, true);
CONFIG_VARIABLE(WeaponOutlineGlowEnabled, bool, true);
