#pragma once

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/HudInWorldPanelZOrder.h>

#include "Details/SoundVisualizationFeature.h"
#include "Details/SoundVisualizationPanelProperties.h"

struct BombDefusePanels {
    [[nodiscard]] static cs2::CPanel2D* createContainerPanel(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        return inWorldFactory.createPanel("BombDefuseContainer", HudInWorldPanelZOrder::BombDefuse);
    }

    [[nodiscard]] static SoundVisualizationPanelProperties soundVisualizationPanelProperties() noexcept
    {
        return SoundVisualizationPanelProperties{
            .svgImagePath = "s2r://panorama/images/icons/equipment/defuser.vsvg",
            .svgTextureHeight = 40,
            .position = SoundVisualizationPosition::AtOrigin};
    }
};

using BombDefuseVisualizerState = SoundVisualizationFeatureState<BombDefusePanels>;
using BombDefuseVisualizer = SoundVisualizationFeature<BombDefusePanels, BombDefuseSound>;
