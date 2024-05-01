#pragma once

#include <cstdint>

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanelIndex.h>
#include <Utils/DynamicArray.h>

enum class PlayerHealthTextColor : std::uint8_t {
    HealthBased,
    White
};

enum class PlayerPositionArrowColor : std::uint8_t {
    PlayerOrTeamColor,
    TeamColor
};

struct PlayerInformationThroughWallsState {
    bool enabled{false};
    bool showOnlyEnemies{false};
    bool showPlayerPosition{true};
    PlayerPositionArrowColor playerPositionArrowColor{PlayerPositionArrowColor::PlayerOrTeamColor};
    bool showPlayerHealth{true};
    PlayerHealthTextColor playerHealthTextColor{PlayerHealthTextColor::HealthBased};
    bool showPlayerActiveWeapon{true};

    cs2::PanelHandle containerPanelHandle;
    DynamicArray<HudInWorldPanelIndex> panelIndices;
};
