#pragma once

#include <CS2/Classes/Panorama.h>

#include "Details/SoundVisualizationFeature.h"
#include "Details/SoundVisualizationPanelProperties.h"

struct FootstepPanels {
    [[nodiscard]] static SoundVisualizationPanelProperties soundVisualizationPanelProperties() noexcept
    {
        return SoundVisualizationPanelProperties{
            .svgImagePath = "s2r://panorama/images/icons/equipment/stomp_damage.svg",
            .position = SoundVisualizationPosition::AboveOrigin};
    }
};

using FootstepVisualizer = SoundVisualizationFeature<FootstepPanels, FootstepSound>;
