#pragma once

#include "Details/WeaponReloadSound.h"
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

template <typename HookContext>
using WeaponReloadVisualizer = SoundVisualizationFeature<HookContext, WeaponReloadPanels, WeaponReloadSound>;
