#pragma once

#include <algorithm>

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/SoundNames.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
#include <FeatureHelpers/Sound/BombPlantVisualizerHelpers.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/WorldToClipSpaceConverter.h>
#include <GameClasses/Panel.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaTransformFactory.h>
#include <Hooks/ViewRenderHook.h>

#include "FootstepVisualizer.h"

struct BombPlantPanels {
    void createBombPlantPanels(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        if (bombPlantContainerPanelPointer.get())
            return;

        const auto bombPlantContainer = inWorldFactory.createPanel("BombPlantContainer");
        if (!bombPlantContainer)
            return;

        bombPlantContainerPanelPointer = bombPlantContainer->uiPanel;

        for (std::size_t i = 0; i < kMaxNumberOfBombPlantsToDraw; ++i) {
            PanoramaUiEngine::runScript(bombPlantContainer->uiPanel,
                R"(
(function() {
var bombPlantPanel = $.CreatePanel('Panel', $.GetContextPanel().FindChildInLayoutFile("BombPlantContainer"), '', {
  style: 'width: 100px; height: 100px; x: -50px; y: -100px; transform-origin: 50% 100%;'
});

$.CreatePanel('Image', bombPlantPanel, '', {
  src: "s2r://panorama/images/icons/ui/chatwheel_bombat.svg",
  style: "horizontal-align: center; vertical-align: center; img-shadow: 0px 0px 1px 3 #000000;",
  textureheight: "64"
});
})();)", "", 0);
        }
    }

    [[nodiscard]] PanoramaUiPanel getBombPlantPanel(std::size_t index) const noexcept
    {
        const auto bombPlantContainerPanel = bombPlantContainerPanelPointer.get();
        if (!bombPlantContainerPanel)
            return PanoramaUiPanel{ nullptr };

        if (const auto children = bombPlantContainerPanel.children()) {
            if (children->size > 0 && static_cast<std::size_t>(children->size) > index)
                return PanoramaUiPanel{ children->memory[index] };
        }
        return PanoramaUiPanel{ nullptr };
    }

    void hidePanels(std::size_t fromPanelIndex) const noexcept
    {
        for (std::size_t i = fromPanelIndex; i < kMaxNumberOfBombPlantsToDraw; ++i) {
            const auto panel = getBombPlantPanel(i);
            if (!panel)
                break;

            if (const auto style = panel.getStyle())
                style.setOpacity(0.0f);
        }
    }

    PanoramaPanelPointer bombPlantContainerPanelPointer;

private:
    static constexpr auto kMaxNumberOfBombPlantsToDraw = 5;
};

class BombPlantVisualizer : public TogglableFeature<BombPlantVisualizer> {
public:
    explicit BombPlantVisualizer(ViewRenderHook& viewRenderHook, SoundWatcher& soundWatcher) noexcept
        : viewRenderHook{ viewRenderHook }
        , soundWatcher{ soundWatcher }
    {
    }

    ~BombPlantVisualizer() noexcept
    {
        if (panels.bombPlantContainerPanelPointer.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(panels.bombPlantContainerPanelPointer.getHandle());
    }

    void run(const BombPlantVisualizerHelpers& params) noexcept
    {
        if (!isEnabled())
            return;

        if (!params.globalVarsProvider || !params.globalVarsProvider.getGlobalVars())
            return;

        if (!params.worldtoClipSpaceConverter)
            return;

        panels.createBombPlantPanels(params.hudInWorldPanelFactory);

        std::size_t currentIndex = 0;
        std::as_const(soundWatcher).getSoundsOfType<BombPlantSound>().forEach([this, &currentIndex, params] (const PlayedSound& sound) {
            const auto soundInClipSpace = params.worldtoClipSpaceConverter.toClipSpace(sound.origin);
            if (!soundInClipSpace.onScreen())
                return;
            
            const auto deviceCoordinates = soundInClipSpace.toNormalizedDeviceCoordinates();
            const auto panel = panels.getBombPlantPanel(currentIndex);
            if (!panel)
                return;

            const auto style = panel.getStyle();
            if (!style)
                return;

            style.setOpacity(BombPlantSound::getOpacity(sound.getTimeAlive(params.globalVarsProvider.getGlobalVars()->curtime)));

            cs2::CTransform3D* transformations[]{ params.transformFactory.create<cs2::CTransformScale3D>(
                BombPlantSound::getScale(soundInClipSpace.z), BombPlantSound::getScale(soundInClipSpace.z), 1.0f
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
        soundWatcher.startWatching<BombPlantSound>();
    }

    void onDisable() noexcept
    {
        viewRenderHook.decrementReferenceCount();
        soundWatcher.stopWatching<BombPlantSound>();
        panels.hidePanels(0);
    }

    BombPlantPanels panels;
    ViewRenderHook& viewRenderHook;
    SoundWatcher& soundWatcher;
};
