#pragma once

#include <Config/ConfigVariable.h>
#include "PlayerPositionArrow/PlayerPositionArrowColorType.h"

enum class PlayerHealthTextColor : std::uint8_t {
    HealthBased,
    White
};

CONFIG_VARIABLE(PlayerInfoInWorldEnabled, bool, false);
CONFIG_VARIABLE(PlayerInfoInWorldOnlyEnemies, bool, false);
CONFIG_VARIABLE(PlayerInfoInWorldPlayerPositionArrowEnabled, bool, true);
CONFIG_VARIABLE(PlayerInfoInWorldPlayerPositionArrowColorMode, PlayerPositionArrowColorType, PlayerPositionArrowColorType::PlayerOrTeamColor);
CONFIG_VARIABLE(PlayerInfoInWorldPlayerHealthEnabled, bool, true);
CONFIG_VARIABLE(PlayerInfoInWorldPlayerHealthColorMode, PlayerHealthTextColor, PlayerHealthTextColor::HealthBased);
CONFIG_VARIABLE(PlayerInfoInWorldActiveWeaponIconEnabled, bool, true);
CONFIG_VARIABLE(PlayerInfoInWorldActiveWeaponAmmoEnabled, bool, true);
CONFIG_VARIABLE(PlayerInfoInWorldBombCarrierIconEnabled, bool, true);
CONFIG_VARIABLE(PlayerInfoInWorldBombPlantIconEnabled, bool, true);
CONFIG_VARIABLE(PlayerInfoInWorldBombDefuseIconEnabled, bool, true);
CONFIG_VARIABLE(PlayerInfoInWorldHostagePickupIconEnabled, bool, true);
CONFIG_VARIABLE(PlayerInfoInWorldHostageRescueIconEnabled, bool, true);
CONFIG_VARIABLE(PlayerInfoInWorldBlindedIconEnabled, bool, true);
