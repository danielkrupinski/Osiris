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
