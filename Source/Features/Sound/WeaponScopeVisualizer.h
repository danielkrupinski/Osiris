#pragma once

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/HudInWorldPanelZOrder.h>

#include "Details/SoundVisualizationFeature.h"
#include "Details/SoundVisualizationPanelProperties.h"

struct WeaponScopePanels {
    [[nodiscard]] static cs2::CPanel2D* createContainerPanel(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        return inWorldFactory.createPanel("WeaponScopeContainer", HudInWorldPanelZOrder::WeaponScope);
    }

    [[nodiscard]] static SoundVisualizationPanelProperties soundVisualizationPanelProperties() noexcept
    {
        return SoundVisualizationPanelProperties{
            .svgImagePath = "s2r://panorama/images/icons/ui/chatwheel_sniperspotted.svg",
            .position = SoundVisualizationPosition::AtOrigin};
    }
};

using WeaponScopeVisualizerState = SoundVisualizationFeatureState<WeaponScopePanels>;
using WeaponScopeVisualizer = SoundVisualizationFeature<WeaponScopePanels, WeaponScopeSound>;
