#pragma once

#include <CS2/Classes/Panorama.h>

#include "Details/SoundVisualizationFeature.h"
#include "Details/SoundVisualizationPanelProperties.h"

struct WeaponReloadPanels {
    [[nodiscard]] static SoundVisualizationPanelProperties soundVisualizationPanelProperties() noexcept
    {
        return SoundVisualizationPanelProperties{
            .svgImagePath = "s2r://panorama/images/icons/ui/switch_teams_dead.svg",
            .position = SoundVisualizationPosition::AtOrigin};
    }
};

using WeaponReloadVisualizer = SoundVisualizationFeature<WeaponReloadPanels, WeaponReloadSound>;
