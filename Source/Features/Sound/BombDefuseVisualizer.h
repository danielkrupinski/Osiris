#pragma once

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/PanoramaTransformations.h>
#include <FeatureHelpers/Sound/BombDefuseSound.h>
#include <FeatureHelpers/Sound/SoundVisualizationFeature.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <Hooks/ViewRenderHook.h>

struct BombDefusePanels {
    [[nodiscard]] static cs2::CPanel2D* createContainerPanel(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        return inWorldFactory.createPanel("BombDefuseContainer", HudInWorldPanelZOrder::BombDefuse);
    }

    static void createContentPanels(cs2::CUIPanel& containerPanel) noexcept
    {
        for (std::size_t i = 0; i < kMaxNumberOfPanels; ++i) {
            PanoramaUiEngine::runScript(&containerPanel,
                R"(
(function() {
var bombDefusePanel = $.CreatePanel('Panel', $.GetContextPanel().FindChildInLayoutFile("BombDefuseContainer"), '', {
  style: 'width: 100px; height: 100px; x: -50px; y: -50px;'
});

$.CreatePanel('Image', bombDefusePanel, '', {
  src: "s2r://panorama/images/icons/equipment/defuser.vsvg",
  style: "horizontal-align: center; vertical-align: center; img-shadow: 0px 0px 1px 3 #000000;",
  textureheight: "40"
});
})();)", "", 0);
        }
    }

    static constexpr auto kMaxNumberOfPanels = 5;
};

using BombDefuseVisualizer = SoundVisualizationFeature<BombDefusePanels, BombDefuseSound>;
