#pragma once

#include "Details/FootstepSound.h"
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

template <typename HookContext>
using FootstepVisualizer = SoundVisualizationFeature<HookContext, FootstepPanels, FootstepSound>;
