#pragma once

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/PanoramaTransformations.h>
#include <FeatureHelpers/Sound/WeaponScopeSound.h>
#include <FeatureHelpers/Sound/SoundVisualizationFeature.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <Hooks/ViewRenderHook.h>

struct WeaponScopePanels {
    [[nodiscard]] static cs2::CPanel2D* createContainerPanel(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        return inWorldFactory.createPanel("WeaponScopeContainer", HudInWorldPanelZOrder::WeaponScope);
    }

    static void createContentPanels(cs2::CUIPanel& containerPanel) noexcept
    {
        for (std::size_t i = 0; i < kMaxNumberOfPanels; ++i) {
            PanoramaUiEngine::runScript(&containerPanel,
                R"(
(function() {
var weaponScopePanel = $.CreatePanel('Panel', $.GetContextPanel().FindChildInLayoutFile("WeaponScopeContainer"), '', {
  style: 'width: 100px; height: 100px; x: -50px; y: -50px;'
});

$.CreatePanel('Image', weaponScopePanel, '', {
  src: "s2r://panorama/images/icons/ui/chatwheel_sniperspotted.svg",
  style: "horizontal-align: center; vertical-align: center; img-shadow: 0px 0px 1px 3 #000000;"
});
})();)", "", 0);
        }
    }

    static constexpr auto kMaxNumberOfPanels = 5;
};

using WeaponScopeVisualizer = SoundVisualizationFeature<WeaponScopePanels, WeaponScopeSound>;
