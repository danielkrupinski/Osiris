#pragma once

#include <cstdint>

#include <Config/ConfigVariable.h>
#include <Utils/ColorUtils.h>
#include <Utils/IdentityMacro.h>
#include <Utils/InRange.h>
#include "ModelGlowParams.h"

enum class PlayerModelGlowColorType : std::uint8_t {
    PlayerOrTeamColor,
    TeamColor,
    HealthBased,
    EnemyAlly
};

namespace model_glow_vars
{

CONFIG_VARIABLE(Enabled, bool, false);
CONFIG_VARIABLE(GlowPlayers, bool, true);
CONFIG_VARIABLE(PlayerGlowColorMode, PlayerModelGlowColorType, PlayerModelGlowColorType::PlayerOrTeamColor);
CONFIG_VARIABLE(GlowOnlyEnemies, bool, true);
CONFIG_VARIABLE(GlowWeapons, bool, true);
CONFIG_VARIABLE(GlowDroppedBomb, bool, true);
CONFIG_VARIABLE(GlowTickingBomb, bool, true);
CONFIG_VARIABLE(GlowDefuseKits, bool, true);
CONFIG_VARIABLE(GlowGrenadeProjectiles, bool, true);

CONFIG_VARIABLE_HUE(PlayerBlueHue, model_glow_params::kPlayerBlueHue);
CONFIG_VARIABLE_HUE(PlayerGreenHue, model_glow_params::kPlayerGreenHue);
CONFIG_VARIABLE_HUE(PlayerYellowHue, model_glow_params::kPlayerYellowHue);
CONFIG_VARIABLE_HUE(PlayerOrangeHue, model_glow_params::kPlayerOrangeHue);
CONFIG_VARIABLE_HUE(PlayerPurpleHue, model_glow_params::kPlayerPurpleHue);

CONFIG_VARIABLE_HUE(TeamTHue, model_glow_params::kTeamTHue);
CONFIG_VARIABLE_HUE(TeamCTHue, model_glow_params::kTeamCTHue);

CONFIG_VARIABLE_HUE(LowHealthHue, model_glow_params::kLowHealthHue);
CONFIG_VARIABLE_HUE(HighHealthHue, model_glow_params::kHighHealthHue);

CONFIG_VARIABLE_HUE(AllyHue, model_glow_params::kAllyHue);
CONFIG_VARIABLE_HUE(EnemyHue, model_glow_params::kEnemyHue);

CONFIG_VARIABLE_HUE(MolotovHue, model_glow_params::kMolotovHue);
CONFIG_VARIABLE_HUE(FlashbangHue, model_glow_params::kFlashbangHue);
CONFIG_VARIABLE_HUE(HEGrenadeHue, model_glow_params::kHEGrenadeHue);
CONFIG_VARIABLE_HUE(SmokeGrenadeHue, model_glow_params::kSmokeGrenadeHue);

CONFIG_VARIABLE_HUE(DroppedBombHue, model_glow_params::kDroppedBombHue);
CONFIG_VARIABLE_HUE(TickingBombHue, model_glow_params::kTickingBombHue);
CONFIG_VARIABLE_HUE(DefuseKitHue, model_glow_params::kDefuseKitHue);

}
