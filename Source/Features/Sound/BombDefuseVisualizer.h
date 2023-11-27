#pragma once

#include <CS2/Classes/Panorama.h>
#include <FeatureHelpers/HudInWorldPanels.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/PanoramaTransformations.h>
#include <FeatureHelpers/Sound/BombDefuseSound.h>
#include <FeatureHelpers/Sound/BombDefuseVisualizerHelpers.h>
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

class BombDefuseVisualizer : public TogglableFeature<BombDefuseVisualizer> {
public:
    explicit BombDefuseVisualizer(ViewRenderHook& viewRenderHook, SoundWatcher& soundWatcher) noexcept
        : viewRenderHook{ viewRenderHook }
        , soundWatcher{ soundWatcher }
    {
    }

    void run(const BombDefuseVisualizerHelpers& params) noexcept
    {
        if (!isEnabled())
            return;

        if (!params.globalVarsProvider || !params.globalVarsProvider.getGlobalVars())
            return;

        if (!params.worldtoClipSpaceConverter)
            return;

        panels.createPanels(params.hudInWorldPanelFactory);

        std::size_t currentIndex = 0;
        std::as_const(soundWatcher).getSoundsOfType<BombDefuseSound>().forEach([this, &currentIndex, params](const PlayedSound& sound) {
            const auto soundInClipSpace = params.worldtoClipSpaceConverter.toClipSpace(sound.origin);
            if (!soundInClipSpace.onScreen())
                return;

            const auto opacity = BombDefuseSound::getOpacity(sound.getTimeAlive(params.globalVarsProvider.getGlobalVars()->curtime));
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
                BombDefuseSound::getScale(soundInClipSpace.z), BombDefuseSound::getScale(soundInClipSpace.z), 1.0f
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
        soundWatcher.startWatching<BombDefuseSound>();
    }

    void onDisable() noexcept
    {
        viewRenderHook.decrementReferenceCount();
        soundWatcher.stopWatching<BombDefuseSound>();
        panels.hidePanels(0);
    }

    HudInWorldPanels<BombDefusePanels> panels;
    ViewRenderHook& viewRenderHook;
    SoundWatcher& soundWatcher;
};
