#pragma once

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/HudInWorldPanelZOrder.h>
#include <FeatureHelpers/Sound/WeaponReloadSound.h>
#include <FeatureHelpers/Sound/SoundVisualizationFeature.h>
#include <GameClasses/PanoramaUiEngine.h>

struct WeaponReloadPanels {
    [[nodiscard]] static cs2::CPanel2D* createContainerPanel(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        return inWorldFactory.createPanel("WeaponReloadContainer", HudInWorldPanelZOrder::WeaponReload);
    }

    static void createContentPanels(cs2::CUIPanel& containerPanel) noexcept
    {
        for (std::size_t i = 0; i < kMaxNumberOfPanels; ++i) {
            PanoramaUiEngine::runScript(&containerPanel,
                R"(
(function() {
var weaponReloadPanel = $.CreatePanel('Panel', $.GetContextPanel().FindChildInLayoutFile("WeaponReloadContainer"), '', {
  style: 'width: 100px; height: 100px; x: -50px; y: -50px;'
});

$.CreatePanel('Image', weaponReloadPanel, '', {
  src: "s2r://panorama/images/icons/ui/switch_teams_dead.svg",
  style: "horizontal-align: center; vertical-align: center; img-shadow: 0px 0px 1px 3 #000000;"
});
})();)", "", 0);
        }
    }

    static constexpr auto kMaxNumberOfPanels = 10;
};

using WeaponReloadVisualizer = SoundVisualizationFeature<WeaponReloadPanels, WeaponReloadSound>;
