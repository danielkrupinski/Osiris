#pragma once

#include <cstdint>

#include "PlayerPositionArrow/PlayerPositionArrowColorType.h"
#include "PlayerStateIcons/PlayerStateIconsToShow.h"

enum class PlayerHealthTextColor : std::uint8_t {
    HealthBased,
    White
};

struct PlayerInfoInWorldState {
    bool enabled{false};
    bool showOnlyEnemies{false};
    bool showPlayerPosition{true};
    PlayerPositionArrowColorType playerPositionArrowColor{PlayerPositionArrowColorType::PlayerOrTeamColor};
    bool showPlayerHealth{true};
    PlayerHealthTextColor playerHealthTextColor{PlayerHealthTextColor::HealthBased};
    bool showPlayerActiveWeapon{true};
    bool showBombCarrierIcon{true};
    bool showBombPlantingIcon{true};
    bool showPlayerActiveWeaponAmmo{true};
    PlayerStateIconsToShow playerStateIconsToShow{
        PlayerStateIconsToShow{}
            .set<DefuseIconPanel>()
            .set<HostagePickupPanel>()
            .set<HostageRescuePanel>()
            .set<BlindedIconPanel>()};
};
