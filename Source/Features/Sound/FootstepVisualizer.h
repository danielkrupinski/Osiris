#pragma once

#include <algorithm>
#include <array>

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/SoundNames.h>
#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/PanoramaTransformations.h>
#include <FeatureHelpers/Sound/FootstepSound.h>
#include <FeatureHelpers/Sound/SoundVisualizationFeature.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/WorldToClipSpaceConverter.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaTransformFactory.h>
#include <Hooks/ViewRenderHook.h>

struct FootstepPanels {
    static constexpr auto kMaxNumberOfPanels = 100;

    [[nodiscard]] static cs2::CPanel2D* createContainerPanel(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        return inWorldFactory.createPanel("FootstepContainer", HudInWorldPanelZOrder::Footstep);
    }

    static void createContentPanels(cs2::CUIPanel& containerPanel) noexcept
    {
        for (std::size_t i = 0; i < kMaxNumberOfPanels; ++i) {
            PanoramaUiEngine::runScript(&containerPanel,
                R"(
(function() {
var footstepPanel = $.CreatePanel('Panel', $.GetContextPanel().FindChildInLayoutFile("FootstepContainer"), '', {
  style: 'width: 50px; height: 50px; x: -25px; y: -50px; transform-origin: 50% 100%;'
});

$.CreatePanel('Image', footstepPanel, '', {
  src: "s2r://panorama/images/icons/equipment/stomp_damage.svg",
  style: "horizontal-align: center; vertical-align: bottom; img-shadow: 0px 0px 1px 3 #000000;"
});
})();)", "", 0);
        }
    }
};

using FootstepVisualizer = SoundVisualizationFeature<FootstepPanels, FootstepSound>;
