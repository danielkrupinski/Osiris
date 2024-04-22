#pragma once

#include <cstdint>

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanelIndex.h>
#include <Utils/DynamicArray.h>

enum class PlayerHealthTextColor : std::uint8_t {
    HealthBased,
    White
};

struct PlayerInformationThroughWallsState {
    bool enabled{false};
    bool showOnlyEnemies{false};
    bool showPlayerPosition{false};
    bool showPlayerHealth{false};
    PlayerHealthTextColor playerHealthTextColor{PlayerHealthTextColor::HealthBased};
    bool showPlayerActiveWeapon{false};

    cs2::PanelHandle containerPanelHandle;
    DynamicArray<HudInWorldPanelIndex> panelIndices;
};
