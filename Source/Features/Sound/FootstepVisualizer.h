#pragma once

#include <algorithm>
#include <array>

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/SoundNames.h>
#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/PanoramaTransformations.h>
#include <FeatureHelpers/Sound/FootstepSound.h>
#include <FeatureHelpers/Sound/FootstepVisualizerHelpers.h>
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

class FootstepVisualizer : public TogglableFeature<FootstepVisualizer> {
public:
    explicit FootstepVisualizer(ViewRenderHook& viewRenderHook, SoundWatcher& soundWatcher) noexcept
        : viewRenderHook{ viewRenderHook }
        , soundWatcher{ soundWatcher }
    {
    }

    void run(const FootstepVisualizerHelpers& params) noexcept
    {
        if (!isEnabled())
            return;

        if (!params.globalVarsProvider || !params.globalVarsProvider.getGlobalVars())
            return;

        if (!params.worldtoClipSpaceConverter)
            return;

        panels.createPanels(params.hudInWorldPanelFactory);

        std::size_t currentIndex = 0;
        std::as_const(soundWatcher).getSoundsOfType<FootstepSound>().forEach([this, &currentIndex, params] (const PlayedSound& sound) {
            const auto soundInClipSpace = params.worldtoClipSpaceConverter.toClipSpace(sound.origin);
            if (!soundInClipSpace.onScreen())
                return;
            
            const auto opacity = FootstepSound::getOpacity(sound.getTimeAlive(params.globalVarsProvider.getGlobalVars()->curtime));
            if (opacity <= 0.0f)
                return;

            const auto panel = panels.getPanel(currentIndex);
            if (!panel)
                return;

            const auto style = panel.getStyle();
            if (!style)
                return;

            style.setOpacity(opacity);
            style.setZIndex(-soundInClipSpace.z);

            const auto deviceCoordinates = soundInClipSpace.toNormalizedDeviceCoordinates();
            PanoramaTransformations{ params.transformFactory.create<cs2::CTransformScale3D>(
                FootstepSound::getScale(soundInClipSpace.z), FootstepSound::getScale(soundInClipSpace.z), 1.0f
            ), params.transformFactory.create<cs2::CTransformTranslate3D>(
                deviceCoordinates.getX(),
                deviceCoordinates.getY(),
                cs2::CUILength{ 0.0f, cs2::CUILength::k_EUILengthLength }
            ) }.applyTo(style);

            ++currentIndex;
        });

        panels.hidePanels(currentIndex);
    }

private:
    friend TogglableFeature;

    void onEnable() noexcept
    {
        viewRenderHook.incrementReferenceCount();
        soundWatcher.startWatching<FootstepSound>();
    }

    void onDisable() noexcept
    {
        viewRenderHook.decrementReferenceCount();
        soundWatcher.stopWatching<FootstepSound>();
        panels.hidePanels(0);
    }

    HudInWorldPanels<FootstepPanels> panels;
    ViewRenderHook& viewRenderHook;
    SoundWatcher& soundWatcher;
};
