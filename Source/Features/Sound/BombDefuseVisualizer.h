#pragma once

#include "Details/BombDefuseSound.h"
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

template <typename HookContext>
using BombDefuseVisualizer = SoundVisualizationFeature<HookContext, BombDefusePanels, BombDefuseSound>;
