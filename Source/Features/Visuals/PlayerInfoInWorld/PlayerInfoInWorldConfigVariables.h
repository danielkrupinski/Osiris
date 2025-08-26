#pragma once

#include <Config/ConfigVariable.h>
#include "PlayerPositionArrow/PlayerPositionArrowColorType.h"

enum class PlayerHealthTextColor : std::uint8_t {
    HealthBased,
    White
};

namespace player_info_vars
{

CONFIG_VARIABLE(Enabled, bool, false);
CONFIG_VARIABLE(OnlyEnemies, bool, false);
CONFIG_VARIABLE(PlayerPositionArrowEnabled, bool, true);
CONFIG_VARIABLE(PlayerPositionArrowColorMode, PlayerPositionArrowColorType, PlayerPositionArrowColorType::PlayerOrTeamColor);
CONFIG_VARIABLE(PlayerHealthEnabled, bool, true);
CONFIG_VARIABLE(PlayerHealthColorMode, PlayerHealthTextColor, PlayerHealthTextColor::HealthBased);
CONFIG_VARIABLE(ActiveWeaponIconEnabled, bool, true);
CONFIG_VARIABLE(ActiveWeaponAmmoEnabled, bool, true);
CONFIG_VARIABLE(BombCarrierIconEnabled, bool, true);
CONFIG_VARIABLE(BombPlantIconEnabled, bool, true);
CONFIG_VARIABLE(BombDefuseIconEnabled, bool, true);
CONFIG_VARIABLE(HostagePickupIconEnabled, bool, true);
CONFIG_VARIABLE(HostageRescueIconEnabled, bool, true);
CONFIG_VARIABLE(BlindedIconEnabled, bool, true);

}
