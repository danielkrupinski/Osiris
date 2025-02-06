#pragma once

#include "Details/BombBeepSound.h"
#include "Details/SoundVisualizationFeature.h"
#include "Details/SoundVisualizationPanelProperties.h"

struct BombBeepPanels {
    [[nodiscard]] static SoundVisualizationPanelProperties soundVisualizationPanelProperties() noexcept
    {
        return SoundVisualizationPanelProperties{
            .svgImagePath = "s2r://panorama/images/icons/ui/bomb_c4.svg",
            .svgTextureHeight = 40,
            .position = SoundVisualizationPosition::AtOrigin};
    }
};

template <typename HookContext>
using BombBeepVisualizer = SoundVisualizationFeature<HookContext, BombBeepPanels, BombBeepSound>;
