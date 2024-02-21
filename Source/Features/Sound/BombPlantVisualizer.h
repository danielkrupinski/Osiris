#pragma once

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/HudInWorldPanelZOrder.h>

#include "Details/SoundVisualizationFeature.h"
#include "Details/SoundVisualizationPanelProperties.h"

struct BombPlantPanels {
    [[nodiscard]] static cs2::CPanel2D* createContainerPanel(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        return inWorldFactory.createPanel("BombPlantContainer", HudInWorldPanelZOrder::BombPlant);
    }

    [[nodiscard]] static SoundVisualizationPanelProperties soundVisualizationPanelProperties() noexcept
    {
        return SoundVisualizationPanelProperties{
            .svgImagePath = "s2r://panorama/images/icons/ui/chatwheel_bombat.svg",
            .svgTextureHeight = 64,
            .position = SoundVisualizationPosition::AboveOrigin};
    }
};

using BombPlantVisualizerState = SoundVisualizationFeatureState<BombPlantPanels>;
using BombPlantVisualizer = SoundVisualizationFeature<BombPlantPanels, BombPlantSound>;
