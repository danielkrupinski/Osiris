#pragma once

#include <array>

#include <CS2/Panorama/PanelHandle.h>
#include <Features/Sound/SoundVisualizationPanelTypes.h>
#include <Utils/DynamicArray.h>

#include "InWorldPanelIndex.h"
#include "InWorldPanelListEntry.h"

struct InWorldPanelsState {
    cs2::PanelHandle containerPanelHandle;
    DynamicArray<InWorldPanelListEntry> panelList;
    InWorldPanelIndex playerInfoPanelListHead{};
    std::array<InWorldPanelIndex, SoundVisualizationPanelTypes::size()> soundVisualizationPanelListHeads{};

    void reset() noexcept
    {
        containerPanelHandle = {};
        panelList.clear();
        playerInfoPanelListHead = {};
        soundVisualizationPanelListHeads = {};
    }
};
