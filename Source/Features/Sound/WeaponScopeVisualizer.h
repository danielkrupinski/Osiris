#pragma once

#include "Details/WeaponScopeSound.h"
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

template <typename HookContext>
using WeaponScopeVisualizer = SoundVisualizationFeature<HookContext, WeaponScopePanels, WeaponScopeSound>;
