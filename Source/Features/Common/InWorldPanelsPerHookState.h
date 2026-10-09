#pragma once

#include <array>
#include <Features/Sound/SoundVisualizationPanelTypes.h>
#include "InWorldPanelIndex.h"

struct InWorldPanelsPerHookState {
    void reset() noexcept
    {
        lastUsedPlayerInfoPanelIndex = {};
        lastUsedSoundVisualizationPanelIndexes = {};
    }

    InWorldPanelIndex lastUsedPlayerInfoPanelIndex{};
    std::array<InWorldPanelIndex, SoundVisualizationPanelTypes::size()> lastUsedSoundVisualizationPanelIndexes{};
};
