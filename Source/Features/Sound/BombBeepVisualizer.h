#pragma once

#include <algorithm>

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/SoundNames.h>
#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/HudInWorldPanelZOrder.h>
#include <FeatureHelpers/Sound/BombBeepVisualizerHelpers.h>
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

class BombBeepVisualizer : public TogglableFeature<BombBeepVisualizer> {
public:
    explicit BombBeepVisualizer(ViewRenderHook& viewRenderHook, SoundWatcher& soundWatcher) noexcept
        : viewRenderHook{ viewRenderHook }
        , soundWatcher{ soundWatcher }
    {
    }

    void run(const BombBeepVisualizerHelpers& params) noexcept
    {
        if (!isEnabled())
            return;

        if (!params.globalVarsProvider || !params.globalVarsProvider.getGlobalVars())
            return;

        if (!params.worldtoClipSpaceConverter)
            return;

        panels.createPanels(params.hudInWorldPanelFactory);

        std::size_t currentIndex = 0;
        std::as_const(soundWatcher).getSoundsOfType<BombBeepSound>().forEach([this, &currentIndex, params](const PlayedSound& sound) {
            const auto soundInClipSpace = params.worldtoClipSpaceConverter.toClipSpace(sound.origin);
            if (!soundInClipSpace.onScreen())
                return;

            const auto opacity = BombBeepSound::getOpacity(sound.getTimeAlive(params.globalVarsProvider.getGlobalVars()->curtime));
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
            cs2::CTransform3D* transformations[]{ params.transformFactory.create<cs2::CTransformScale3D>(
                BombBeepSound::getScale(soundInClipSpace.z), BombBeepSound::getScale(soundInClipSpace.z), 1.0f
            ), params.transformFactory.create<cs2::CTransformTranslate3D>(
                deviceCoordinates.getX(),
                deviceCoordinates.getY(),
                cs2::CUILength{ 0.0f, cs2::CUILength::k_EUILengthLength }
            ) };

            cs2::CUtlVector<cs2::CTransform3D*> dummyVector;
            dummyVector.allocationCount = 2;
            dummyVector.memory = transformations;
            dummyVector.growSize = 0;
            dummyVector.size = 2;

            style.setTransform3D(dummyVector);
            ++currentIndex;
        });

        panels.hidePanels(currentIndex);
    }

private:
    friend TogglableFeature;

    void onEnable() noexcept
    {
        viewRenderHook.incrementReferenceCount();
        soundWatcher.startWatching<BombBeepSound>();
    }

    void onDisable() noexcept
    {
        viewRenderHook.decrementReferenceCount();
        soundWatcher.stopWatching<BombBeepSound>();
        panels.hidePanels(0);
    }

    HudInWorldPanels<BombBeepPanels> panels;
    ViewRenderHook& viewRenderHook;
    SoundWatcher& soundWatcher;
};
