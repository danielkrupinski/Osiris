#pragma once

#include <Config/ConfigVariable.h>

enum class PlayerModelGlowColorType : std::uint8_t {
    PlayerOrTeamColor,
    TeamColor,
    HealthBased
};

CONFIG_VARIABLE(ModelGlowEnabled, bool, false);
CONFIG_VARIABLE(PlayerModelGlowEnabled, bool, true);
CONFIG_VARIABLE(PlayerModelGlowColorMode, PlayerModelGlowColorType, PlayerModelGlowColorType::PlayerOrTeamColor);
CONFIG_VARIABLE(PlayerModelGlowOnlyEnemies, bool, true);
CONFIG_VARIABLE(WeaponModelGlowEnabled, bool, true);
CONFIG_VARIABLE(DroppedBombModelGlowEnabled, bool, true);
CONFIG_VARIABLE(TickingBombModelGlowEnabled, bool, true);
CONFIG_VARIABLE(DefuseKitModelGlowEnabled, bool, true);
CONFIG_VARIABLE(GrenadeProjectileModelGlowEnabled, bool, true);
