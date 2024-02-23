#pragma once

#include <CS2/Classes/Panorama.h>

#include "Details/SoundVisualizationFeature.h"
#include "Details/SoundVisualizationPanelProperties.h"

struct WeaponScopePanels {
    [[nodiscard]] static SoundVisualizationPanelProperties soundVisualizationPanelProperties() noexcept
    {
        return SoundVisualizationPanelProperties{
            .svgImagePath = "s2r://panorama/images/icons/ui/chatwheel_sniperspotted.svg",
            .position = SoundVisualizationPosition::AtOrigin};
    }
};

using WeaponScopeVisualizer = SoundVisualizationFeature<WeaponScopePanels, WeaponScopeSound>;
