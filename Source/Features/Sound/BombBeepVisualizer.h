#pragma once

#include <algorithm>

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/SoundNames.h>
#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/HudInWorldPanelZOrder.h>
#include <FeatureHelpers/PanoramaTransformations.h>
#include <FeatureHelpers/Sound/SoundVisualizationFeature.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/WorldToClipSpaceConverter.h>
#include <GameClasses/Panel.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaPanelPointer.h>
#include <Helpers/PanoramaTransformFactory.h>
#include <Hooks/ViewRenderHook.h>

struct BombBeepPanels {
    [[nodiscard]] static cs2::CPanel2D* createContainerPanel(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        return inWorldFactory.createPanel("BombBeepContainer", HudInWorldPanelZOrder::BombBeep);
    }

    static void createContentPanels(cs2::CUIPanel& containerPanel) noexcept
    {
        for (std::size_t i = 0; i < kMaxNumberOfPanels; ++i) {
            PanoramaUiEngine::runScript(&containerPanel,
                R"(
(function() {
var bombBeepPanel = $.CreatePanel('Panel', $.GetContextPanel().FindChildInLayoutFile("BombBeepContainer"), '', {
  style: 'width: 100px; height: 100px; x: -50px; y: -50px;'
});

$.CreatePanel('Image', bombBeepPanel, '', {
  src: "s2r://panorama/images/icons/ui/bomb_c4.svg",
  style: "horizontal-align: center; vertical-align: center; img-shadow: 0px 0px 1px 3 #000000;",
  textureheight: "40"
});
})();)", "", 0);
        }
    }

    static constexpr auto kMaxNumberOfPanels = 5;
};

using BombBeepVisualizer = SoundVisualizationFeature<BombBeepPanels, BombBeepSound>;
