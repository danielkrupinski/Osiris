#pragma once

#include <Config/ConfigVariable.h>
#include "PlayerOutlineGlow/PlayerOutlineGlowColorType.h"
#include "OutlineGlowParams.h"

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

#define CONFIG_VARIABLE_HUE(name, hueParams) \
CONFIG_VARIABLE(name, \
    IDENTITY(InRange<color::HueInteger, color::HueInteger{hueParams.min}, color::HueInteger{hueParams.max}>), \
    color::HueInteger{hueParams.def})

CONFIG_VARIABLE_HUE(PlayerOutlineGlowPlayerBlueHue, outline_glow_params::kPlayerBlueHue);
CONFIG_VARIABLE_HUE(PlayerOutlineGlowPlayerGreenHue, outline_glow_params::kPlayerGreenHue);
CONFIG_VARIABLE_HUE(PlayerOutlineGlowPlayerYellowHue, outline_glow_params::kPlayerYellowHue);
CONFIG_VARIABLE_HUE(PlayerOutlineGlowPlayerOrangeHue, outline_glow_params::kPlayerOrangeHue);
CONFIG_VARIABLE_HUE(PlayerOutlineGlowPlayerPurpleHue, outline_glow_params::kPlayerPurpleHue);

CONFIG_VARIABLE_HUE(PlayerOutlineGlowTeamTHue, outline_glow_params::kTeamTHue);
CONFIG_VARIABLE_HUE(PlayerOutlineGlowTeamCTHue, outline_glow_params::kTeamCTHue);

CONFIG_VARIABLE_HUE(PlayerOutlineGlowLowHealthHue, outline_glow_params::kLowHealthHue);
CONFIG_VARIABLE_HUE(PlayerOutlineGlowHighHealthHue, outline_glow_params::kHighHealthHue);

CONFIG_VARIABLE_HUE(PlayerOutlineGlowAllyHue, outline_glow_params::kAllyHue);
CONFIG_VARIABLE_HUE(PlayerOutlineGlowEnemyHue, outline_glow_params::kEnemyHue);

CONFIG_VARIABLE_HUE(OutlineGlowMolotovHue, outline_glow_params::kMolotovHue);
CONFIG_VARIABLE_HUE(OutlineGlowFlashbangHue, outline_glow_params::kFlashbangHue);
CONFIG_VARIABLE_HUE(OutlineGlowHEGrenadeHue, outline_glow_params::kHEGrenadeHue);
CONFIG_VARIABLE_HUE(OutlineGlowSmokeGrenadeHue, outline_glow_params::kSmokeGrenadeHue);

CONFIG_VARIABLE_HUE(OutlineGlowDroppedBombHue, outline_glow_params::kDroppedBombHue);
CONFIG_VARIABLE_HUE(OutlineGlowTickingBombHue, outline_glow_params::kTickingBombHue);
CONFIG_VARIABLE_HUE(OutlineGlowDefuseKitHue, outline_glow_params::kDefuseKitHue);
