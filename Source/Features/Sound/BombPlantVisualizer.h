#pragma once

#include "Details/BombPlantSound.h"
#include "Details/SoundVisualizationFeature.h"
#include "Details/SoundVisualizationPanelProperties.h"

struct BombPlantPanels {
    [[nodiscard]] static SoundVisualizationPanelProperties soundVisualizationPanelProperties() noexcept
    {
        return SoundVisualizationPanelProperties{
            .svgImagePath = "s2r://panorama/images/icons/ui/chatwheel_bombat.svg",
            .svgTextureHeight = 64,
            .position = SoundVisualizationPosition::AboveOrigin};
    }
};

template <typename HookContext>
using BombPlantVisualizer = SoundVisualizationFeature<HookContext, BombPlantPanels, BombPlantSound>;
