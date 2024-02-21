#pragma once

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/HudInWorldPanelZOrder.h>

#include "Details/SoundVisualizationFeature.h"
#include "Details/SoundVisualizationPanelProperties.h"

struct WeaponReloadPanels {
    [[nodiscard]] static cs2::CPanel2D* createContainerPanel(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        return inWorldFactory.createPanel("WeaponReloadContainer", HudInWorldPanelZOrder::WeaponReload);
    }

    [[nodiscard]] static SoundVisualizationPanelProperties soundVisualizationPanelProperties() noexcept
    {
        return SoundVisualizationPanelProperties{
            .svgImagePath = "s2r://panorama/images/icons/ui/switch_teams_dead.svg",
            .position = SoundVisualizationPosition::AtOrigin};
    }
};

using WeaponReloadVisualizerState = SoundVisualizationFeatureState<WeaponReloadPanels>;
using WeaponReloadVisualizer = SoundVisualizationFeature<WeaponReloadPanels, WeaponReloadSound>;
