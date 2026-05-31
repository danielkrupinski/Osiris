#pragma once

#include <array>
#include <Features/Sound/SoundVisualizationPanelTypes.h>
#include "InWorldPanelIndex.h"

struct InWorldPanelsPerHookState {
    InWorldPanelIndex lastUsedPlayerInfoPanelIndex{};
    InWorldPanelIndex lastUsedPlayerNamePanelIndex{};
    std::array<InWorldPanelIndex, SoundVisualizationPanelTypes::size()> lastUsedSoundVisualizationPanelIndexes{};
};
