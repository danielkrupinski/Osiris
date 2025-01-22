#pragma once

#include <CS2/Panorama/PanelHandle.h>
#include <Utils/DynamicArray.h>
#include <Features/Sound/SoundVisualizationPanelTypes.h>

#include "InWorldPanelIndex.h"
#include "InWorldPanelListEntry.h"

struct InWorldPanelsState {
    cs2::PanelHandle containerPanelHandle;
    DynamicArray<InWorldPanelListEntry> panelList;
    InWorldPanelIndex playerInfoPanelListHead{};
    std::array<InWorldPanelIndex, SoundVisualizationPanelTypes::size()> soundVisualizationPanelListHeads{};

    void onContainerPanelCreation() noexcept
    {
        panelList.clear();
        playerInfoPanelListHead = {};
        soundVisualizationPanelListHeads = {};
    }
};
