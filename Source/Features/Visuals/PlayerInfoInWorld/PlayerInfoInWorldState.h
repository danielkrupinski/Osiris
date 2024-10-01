#pragma once

#include <cstdint>

#include <CS2/Panorama/PanelHandle.h>
#include <FeatureHelpers/HudInWorldPanelIndex.h>
#include <Utils/DynamicArray.h>

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
    bool showPlayerActiveWeaponAmmo{true};
    PlayerStateIconsToShow playerStateIconsToShow{
        PlayerStateIconsToShow{}
            .set<DefuseIconPanel>()
            .set<HostagePickupPanel>()
            .set<HostageRescuePanel>()
            .set<BlindedIconPanel>()};

    cs2::PanelHandle containerPanelHandle;
    DynamicArray<HudInWorldPanelIndex> panelIndices;
};
