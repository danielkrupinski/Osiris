#pragma once

#include <Config/ConfigVariable.h>
#include "PlayerOutlineGlow/PlayerOutlineGlowColorType.h"
#include "OutlineGlowParams.h"

namespace outline_glow_vars
{

CONFIG_VARIABLE(Enabled, bool, false);
CONFIG_VARIABLE(GlowDefuseKits, bool, true);
CONFIG_VARIABLE(GlowDroppedBomb, bool, true);
CONFIG_VARIABLE(GlowGrenadeProjectiles, bool, true);
CONFIG_VARIABLE(GlowHostages, bool, true);
CONFIG_VARIABLE(GlowPlayers, bool, true);
CONFIG_VARIABLE(GlowOnlyEnemies, bool, true);
CONFIG_VARIABLE(PlayerGlowColorMode, PlayerOutlineGlowColorType, PlayerOutlineGlowColorType::PlayerOrTeamColor);
CONFIG_VARIABLE(GlowTickingBomb, bool, true);
CONFIG_VARIABLE(GlowWeapons, bool, true);

CONFIG_VARIABLE_HUE(PlayerBlueHue, outline_glow_params::kPlayerBlueHue);
CONFIG_VARIABLE_HUE(PlayerGreenHue, outline_glow_params::kPlayerGreenHue);
CONFIG_VARIABLE_HUE(PlayerYellowHue, outline_glow_params::kPlayerYellowHue);
CONFIG_VARIABLE_HUE(PlayerOrangeHue, outline_glow_params::kPlayerOrangeHue);
CONFIG_VARIABLE_HUE(PlayerPurpleHue, outline_glow_params::kPlayerPurpleHue);

CONFIG_VARIABLE_HUE(TeamTHue, outline_glow_params::kTeamTHue);
CONFIG_VARIABLE_HUE(TeamCTHue, outline_glow_params::kTeamCTHue);

CONFIG_VARIABLE_HUE(LowHealthHue, outline_glow_params::kLowHealthHue);
CONFIG_VARIABLE_HUE(HighHealthHue, outline_glow_params::kHighHealthHue);

CONFIG_VARIABLE_HUE(AllyHue, outline_glow_params::kAllyHue);
CONFIG_VARIABLE_HUE(EnemyHue, outline_glow_params::kEnemyHue);

CONFIG_VARIABLE_HUE(MolotovHue, outline_glow_params::kMolotovHue);
CONFIG_VARIABLE_HUE(FlashbangHue, outline_glow_params::kFlashbangHue);
CONFIG_VARIABLE_HUE(HEGrenadeHue, outline_glow_params::kHEGrenadeHue);
CONFIG_VARIABLE_HUE(SmokeGrenadeHue, outline_glow_params::kSmokeGrenadeHue);

CONFIG_VARIABLE_HUE(DroppedBombHue, outline_glow_params::kDroppedBombHue);
CONFIG_VARIABLE_HUE(TickingBombHue, outline_glow_params::kTickingBombHue);
CONFIG_VARIABLE_HUE(DefuseKitHue, outline_glow_params::kDefuseKitHue);

CONFIG_VARIABLE_HUE(HostageHue, outline_glow_params::kHostageHue);

}
