#pragma once

#include <algorithm>

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/SoundNames.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/WorldToClipSpaceConverter.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaTransformFactory.h>
#include <Hooks/ViewRenderHook.h>

class BombPlantVisualizer : public TogglableFeature<BombPlantVisualizer> {
public:
    explicit BombPlantVisualizer(HudProvider hudProvider, GlobalVarsProvider globalVarsProvider, ViewRenderHook& viewRenderHook, SoundWatcher& soundWatcher) noexcept
        : hudProvider{ hudProvider }
        , globalVarsProvider{ globalVarsProvider }
        , viewRenderHook{ viewRenderHook }
        , soundWatcher{ soundWatcher }
    {
    }

    ~BombPlantVisualizer() noexcept
    {
        if (bombPlantContainerPanelPointer.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(bombPlantContainerPanelPointer.getHandle());
    }

    void run() noexcept
    {
        if (!isEnabled())
            return;

        if (!globalVarsProvider || !globalVarsProvider.getGlobalVars())
            return;

        if (!worldtoClipSpaceConverter)
            return;

        createBombPlantPanels();

        std::size_t currentIndex = 0;
        soundWatcher.forEach<WatchedSoundType::BombPlant>([this, &currentIndex] (const PlayedSound& sound) {
            const auto soundInClipSpace = worldtoClipSpaceConverter.toClipSpace(sound.origin);
            if (!soundInClipSpace.onScreen())
                return;
            
            const auto deviceCoordinates = soundInClipSpace.toNormalizedDeviceCoordinates();
            const auto panel = getBombPlantPanel(currentIndex);
            if (!panel)
                return;

            const auto style = panel.getStyle();
            if (!style)
                return;

            const auto timeAlive = sound.getTimeAlive(globalVarsProvider.getGlobalVars()->curtime);
            if (timeAlive >= SoundWatcher::kBombPlantLifespan - kBombPlantFadeAwayDuration) {
                style.setOpacity((SoundWatcher::kBombPlantLifespan - timeAlive) / kBombPlantFadeAwayDuration);
            } else {
                style.setOpacity(1.0f);
            }

            cs2::CTransform3D* transformations[]{ transformFactory.create<cs2::CTransformScale3D>(
                (std::max)(1.0f - soundInClipSpace.z / 1000.0f, 0.4f), (std::max)(1.0f - soundInClipSpace.z / 1000.0f, 0.4f), 1.0f
            ), transformFactory.create<cs2::CTransformTranslate3D>(
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

        hidePanels(currentIndex);
    }

private:
    friend TogglableFeature;

    void onEnable() noexcept
    {
        viewRenderHook.incrementReferenceCount();
        soundWatcher.startWatching<WatchedSoundType::BombPlant>();
    }

    void onDisable() noexcept
    {
        viewRenderHook.decrementReferenceCount();
        soundWatcher.stopWatching<WatchedSoundType::BombPlant>();
        hidePanels(0);
    }

    void createBombPlantPanels() noexcept
    {
        if (bombPlantContainerPanelPointer.get())
            return;

        const auto hudReticle = hudProvider.findChildInLayoutFile(cs2::HudReticle);
        if (!hudReticle)
            return;

        PanoramaUiEngine::runScript(hudReticle,
            R"(
$.CreatePanel('Panel', $.GetContextPanel(), 'BombPlantContainer', {
  style: 'width: 100%; height: 100%;'
});)", "", 0);

        bombPlantContainerPanelPointer = hudReticle.findChildInLayoutFile("BombPlantContainer");

        for (std::size_t i = 0; i < kMaxNumberOfBombPlantsToDraw; ++i) {
            PanoramaUiEngine::runScript(hudReticle,
                R"(
(function() {
var bombPlantPanel = $.CreatePanel('Panel', $.GetContextPanel().FindChildInLayoutFile("BombPlantContainer"), '', {
  style: 'width: 100px; height: 100px; x: -50px; y: -50px;'
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
            return PanoramaUiPanel{nullptr};

        if (const auto children = bombPlantContainerPanel.children()) {
            if (children->size > 0 && static_cast<std::size_t>(children->size) > index)
                return PanoramaUiPanel{ children->memory[index] };
        }
        return PanoramaUiPanel{nullptr};
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

    HudProvider hudProvider;
    PanoramaPanelPointer bombPlantContainerPanelPointer;
    GlobalVarsProvider globalVarsProvider;
    PanoramaTransformFactory transformFactory;
    ViewRenderHook& viewRenderHook;
    SoundWatcher& soundWatcher;
    WorldToClipSpaceConverter worldtoClipSpaceConverter;

    static constexpr auto kMaxNumberOfBombPlantsToDraw = 5;
    static constexpr auto kBombPlantFadeAwayDuration = 0.4f;
};
