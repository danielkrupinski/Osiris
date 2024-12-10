#pragma once

#include <Utils/TypeList.h>

struct BombBeepPanels;
struct BombDefusePanels;
struct BombPlantPanels;
struct FootstepPanels;
struct WeaponReloadPanels;
struct WeaponScopePanels;

using SoundVisualizationPanelTypes = TypeList<
    BombBeepPanels,
    BombDefusePanels,
    BombPlantPanels,
    FootstepPanels,
    WeaponReloadPanels,
    WeaponScopePanels>;
