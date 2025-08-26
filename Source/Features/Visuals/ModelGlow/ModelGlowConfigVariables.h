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

CONFIG_VARIABLE(PlayerBlueHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kPlayerBlueMinHue, model_glow_params::kPlayerBlueMaxHue>),
    model_glow_params::kPlayerBlueDefaultHue);

CONFIG_VARIABLE(PlayerGreenHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kPlayerGreenMinHue, model_glow_params::kPlayerGreenMaxHue>),
    model_glow_params::kPlayerGreenDefaultHue);

CONFIG_VARIABLE(PlayerYellowHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kPlayerYellowMinHue, model_glow_params::kPlayerYellowMaxHue>),
    model_glow_params::kPlayerYellowDefaultHue);

CONFIG_VARIABLE(PlayerOrangeHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kPlayerOrangeMinHue, model_glow_params::kPlayerOrangeMaxHue>),
    model_glow_params::kPlayerOrangeDefaultHue);

CONFIG_VARIABLE(PlayerPurpleHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kPlayerPurpleMinHue, model_glow_params::kPlayerPurpleMaxHue>),
    model_glow_params::kPlayerPurpleDefaultHue);

CONFIG_VARIABLE(TeamTHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kTeamTMinHue, model_glow_params::kTeamTMaxHue>),
    model_glow_params::kTeamTDefaultHue);

CONFIG_VARIABLE(TeamCTHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kTeamCTMinHue, model_glow_params::kTeamCTMaxHue>),
    model_glow_params::kTeamCTDefaultHue);

CONFIG_VARIABLE(LowHealthHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kLowHealthMinHue, model_glow_params::kLowHealthMaxHue>),
    model_glow_params::kLowHealthDefaultHue);

CONFIG_VARIABLE(HighHealthHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kHighHealthMinHue, model_glow_params::kHighHealthMaxHue>),
    model_glow_params::kHighHealthDefaultHue);

CONFIG_VARIABLE(AllyHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kAllyMinHue, model_glow_params::kAllyMaxHue>),
    model_glow_params::kAllyDefaultHue);

CONFIG_VARIABLE(EnemyHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kEnemyMinHue, model_glow_params::kEnemyMaxHue>),
    model_glow_params::kEnemyDefaultHue);

CONFIG_VARIABLE(MolotovHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kMolotovMinHue, model_glow_params::kMolotovMaxHue>),
    model_glow_params::kMolotovDefaultHue);

CONFIG_VARIABLE(FlashbangHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kFlashbangMinHue, model_glow_params::kFlashbangMaxHue>),
    model_glow_params::kFlashbangDefaultHue);

CONFIG_VARIABLE(HEGrenadeHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kHEGrenadeMinHue, model_glow_params::kHEGrenadeMaxHue>),
    model_glow_params::kHEGrenadeDefaultHue);

CONFIG_VARIABLE(SmokeGrenadeHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kSmokeGrenadeMinHue, model_glow_params::kSmokeGrenadeMaxHue>),
    model_glow_params::kSmokeGrenadeDefaultHue);

CONFIG_VARIABLE(DroppedBombHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kDroppedBombMinHue, model_glow_params::kDroppedBombMaxHue>),
    model_glow_params::kDroppedBombDefaultHue);

CONFIG_VARIABLE(TickingBombHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kTickingBombMinHue, model_glow_params::kTickingBombMaxHue>),
    model_glow_params::kTickingBombDefaultHue);

CONFIG_VARIABLE(DefuseKitHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kDefuseKitMinHue, model_glow_params::kDefuseKitMaxHue>),
    model_glow_params::kDefuseKitDefaultHue);

}
