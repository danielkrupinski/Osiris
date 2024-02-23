#pragma once

#include <CS2/Classes/Panorama.h>

#include "Details/SoundVisualizationFeature.h"
#include "Details/SoundVisualizationPanelProperties.h"

struct BombDefusePanels {
    [[nodiscard]] static SoundVisualizationPanelProperties soundVisualizationPanelProperties() noexcept
    {
        return SoundVisualizationPanelProperties{
            .svgImagePath = "s2r://panorama/images/icons/equipment/defuser.vsvg",
            .svgTextureHeight = 40,
            .position = SoundVisualizationPosition::AtOrigin};
    }
};

using BombDefuseVisualizer = SoundVisualizationFeature<BombDefusePanels, BombDefuseSound>;
