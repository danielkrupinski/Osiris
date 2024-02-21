#pragma once

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/HudInWorldPanelZOrder.h>

#include "Details/SoundVisualizationFeature.h"
#include "Details/SoundVisualizationPanelProperties.h"

struct FootstepPanels {
    [[nodiscard]] static cs2::CPanel2D* createContainerPanel(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        return inWorldFactory.createPanel("FootstepContainer", HudInWorldPanelZOrder::Footstep);
    }

    [[nodiscard]] static SoundVisualizationPanelProperties soundVisualizationPanelProperties() noexcept
    {
        return SoundVisualizationPanelProperties{
            .svgImagePath = "s2r://panorama/images/icons/equipment/stomp_damage.svg",
            .position = SoundVisualizationPosition::AboveOrigin};
    }
};

using FootstepVisualizerState = SoundVisualizationFeatureState<FootstepPanels>;
using FootstepVisualizer = SoundVisualizationFeature<FootstepPanels, FootstepSound>;
