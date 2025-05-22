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

CONFIG_VARIABLE(ModelGlowEnabled, bool, false);
CONFIG_VARIABLE(PlayerModelGlowEnabled, bool, true);
CONFIG_VARIABLE(PlayerModelGlowColorMode, PlayerModelGlowColorType, PlayerModelGlowColorType::PlayerOrTeamColor);
CONFIG_VARIABLE(PlayerModelGlowOnlyEnemies, bool, true);
CONFIG_VARIABLE(WeaponModelGlowEnabled, bool, true);
CONFIG_VARIABLE(DroppedBombModelGlowEnabled, bool, true);
CONFIG_VARIABLE(TickingBombModelGlowEnabled, bool, true);
CONFIG_VARIABLE(DefuseKitModelGlowEnabled, bool, true);
CONFIG_VARIABLE(GrenadeProjectileModelGlowEnabled, bool, true);

CONFIG_VARIABLE(PlayerModelGlowPlayerBlueHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kPlayerBlueMinHue, model_glow_params::kPlayerBlueMaxHue>),
    model_glow_params::kPlayerBlueDefaultHue);

CONFIG_VARIABLE(PlayerModelGlowPlayerGreenHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kPlayerGreenMinHue, model_glow_params::kPlayerGreenMaxHue>),
    model_glow_params::kPlayerGreenDefaultHue);

CONFIG_VARIABLE(PlayerModelGlowPlayerYellowHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kPlayerYellowMinHue, model_glow_params::kPlayerYellowMaxHue>),
    model_glow_params::kPlayerYellowDefaultHue);

CONFIG_VARIABLE(PlayerModelGlowPlayerOrangeHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kPlayerOrangeMinHue, model_glow_params::kPlayerOrangeMaxHue>),
    model_glow_params::kPlayerOrangeDefaultHue);

CONFIG_VARIABLE(PlayerModelGlowPlayerPurpleHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kPlayerPurpleMinHue, model_glow_params::kPlayerPurpleMaxHue>),
    model_glow_params::kPlayerPurpleDefaultHue);

CONFIG_VARIABLE(PlayerModelGlowTeamTHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kTeamTMinHue, model_glow_params::kTeamTMaxHue>),
    model_glow_params::kTeamTDefaultHue);

CONFIG_VARIABLE(PlayerModelGlowTeamCTHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kTeamCTMinHue, model_glow_params::kTeamCTMaxHue>),
    model_glow_params::kTeamCTDefaultHue);

CONFIG_VARIABLE(PlayerModelGlowLowHealthHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kLowHealthMinHue, model_glow_params::kLowHealthMaxHue>),
    model_glow_params::kLowHealthDefaultHue);

CONFIG_VARIABLE(PlayerModelGlowHighHealthHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kHighHealthMinHue, model_glow_params::kHighHealthMaxHue>),
    model_glow_params::kHighHealthDefaultHue);

CONFIG_VARIABLE(PlayerModelGlowAllyHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kAllyMinHue, model_glow_params::kAllyMaxHue>),
    model_glow_params::kAllyDefaultHue);

CONFIG_VARIABLE(PlayerModelGlowEnemyHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kEnemyMinHue, model_glow_params::kEnemyMaxHue>),
    model_glow_params::kEnemyDefaultHue);

CONFIG_VARIABLE(ModelGlowMolotovHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kMolotovMinHue, model_glow_params::kMolotovMaxHue>),
    model_glow_params::kMolotovDefaultHue);

CONFIG_VARIABLE(ModelGlowFlashbangHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kFlashbangMinHue, model_glow_params::kFlashbangMaxHue>),
    model_glow_params::kFlashbangDefaultHue);

CONFIG_VARIABLE(ModelGlowHEGrenadeHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kHEGrenadeMinHue, model_glow_params::kHEGrenadeMaxHue>),
    model_glow_params::kHEGrenadeDefaultHue);

CONFIG_VARIABLE(ModelGlowSmokeGrenadeHue,
    IDENTITY(InRange<color::HueInteger, model_glow_params::kSmokeGrenadeMinHue, model_glow_params::kSmokeGrenadeMaxHue>),
    model_glow_params::kSmokeGrenadeDefaultHue);
