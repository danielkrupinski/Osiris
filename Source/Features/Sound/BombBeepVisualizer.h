#pragma once

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/HudInWorldPanelZOrder.h>

#include "Details/SoundVisualizationFeature.h"
#include "Details/SoundVisualizationPanelProperties.h"

struct BombBeepPanels {
    [[nodiscard]] static cs2::CPanel2D* createContainerPanel(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        return inWorldFactory.createPanel("BombBeepContainer", HudInWorldPanelZOrder::BombBeep);
    }

    [[nodiscard]] static SoundVisualizationPanelProperties soundVisualizationPanelProperties() noexcept
    {
        return SoundVisualizationPanelProperties{
            .svgImagePath = "s2r://panorama/images/icons/ui/bomb_c4.svg",
            .svgTextureHeight = 40,
            .position = SoundVisualizationPosition::AtOrigin};
    }
};

using BombBeepVisualizerState = SoundVisualizationFeatureState<BombBeepPanels>;
using BombBeepVisualizer = SoundVisualizationFeature<BombBeepPanels, BombBeepSound>;
