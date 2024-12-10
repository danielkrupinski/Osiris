#pragma once

#include <array>
#include <Features/Sound/SoundVisualizationPanelTypes.h>
#include "InWorldPanelIndex.h"

struct InWorldPanelsPerHookState {
    InWorldPanelIndex lastUsedPlayerInfoPanelIndex{};
    std::array<InWorldPanelIndex, SoundVisualizationPanelTypes::size()> lastUsedSoundVisualizationPanelIndexes{};
};
