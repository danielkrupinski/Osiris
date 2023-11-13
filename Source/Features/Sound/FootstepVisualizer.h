#pragma once

#include <algorithm>
#include <array>

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/SoundNames.h>
#include <FeatureHelpers/HudInWorldPanelFactory.h>
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
    static constexpr auto kMaxNumberOfFootstepsToDraw = 100;

    void create(const HudInWorldPanelFactory& inWorldFactory) noexcept
    {
        if (footstepContainerPanelPointer.get())
            return;

        const auto footstepContainer = inWorldFactory.createPanel("FootstepContainer");
        if (!footstepContainer)
            return;

        footstepContainerPanelPointer = footstepContainer->uiPanel;

        for (std::size_t i = 0; i < kMaxNumberOfFootstepsToDraw; ++i) {
            PanoramaUiEngine::runScript(footstepContainer->uiPanel,
                R"(
(function() {
var footstepPanel = $.CreatePanel('Panel', $.GetContextPanel().FindChildInLayoutFile("FootstepContainer"), '', {
  style: 'width: 50px; height: 50px; x: -25px; y: -50px; transform-origin: 50% 100%;'
});

$.CreatePanel('Image', footstepPanel, '', {
  src: "s2r://panorama/images/icons/equipment/stomp_damage.svg",
  style: "horizontal-align: center; vertical-align: center; img-shadow: 0px 0px 1px 3 #000000;"
});
})();)", "", 0);
        }
    }

    [[nodiscard]] PanoramaUiPanel getPanel(std::size_t index) const noexcept
    {
        const auto footstepContainerPanel = footstepContainerPanelPointer.get();
        if (!footstepContainerPanel)
            return PanoramaUiPanel{ nullptr };

        if (const auto children = footstepContainerPanel.children()) {
            if (children->size > 0 && static_cast<std::size_t>(children->size) > index)
                return PanoramaUiPanel{ children->memory[index] };
        }
        return PanoramaUiPanel{ nullptr };
    }

    void hidePanels(std::size_t fromPanelIndex) const noexcept
    {
        for (std::size_t i = fromPanelIndex; i < kMaxNumberOfFootstepsToDraw; ++i) {
            const auto panel = getPanel(i);
            if (!panel)
                break;

            if (const auto style = panel.getStyle())
                style.setOpacity(0.0f);
        }
    }

    PanoramaPanelPointer footstepContainerPanelPointer;
};

class FootstepVisualizer : public TogglableFeature<FootstepVisualizer> {
public:
    explicit FootstepVisualizer(ViewRenderHook& viewRenderHook, SoundWatcher& soundWatcher) noexcept
        : viewRenderHook{ viewRenderHook }
        , soundWatcher{ soundWatcher }
    {
    }

    ~FootstepVisualizer() noexcept
    {
        if (panels.footstepContainerPanelPointer.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(panels.footstepContainerPanelPointer.getHandle());
    }

    void run(const FootstepVisualizerHelpers& params) noexcept
    {
        if (!isEnabled())
            return;

        if (!params.globalVarsProvider || !params.globalVarsProvider.getGlobalVars())
            return;

        if (!params.worldtoClipSpaceConverter)
            return;

        panels.create(params.hudInWorldPanelFactory);

        std::size_t currentIndex = 0;
        std::as_const(soundWatcher).getSoundsOfType<FootstepSound>().forEach([this, &currentIndex, params] (const PlayedSound& sound) {
            const auto soundInClipSpace = params.worldtoClipSpaceConverter.toClipSpace(sound.origin);
            if (!soundInClipSpace.onScreen())
                return;
            
            const auto deviceCoordinates = soundInClipSpace.toNormalizedDeviceCoordinates();
            const auto panel = panels.getPanel(currentIndex);
            if (!panel)
                return;

            const auto style = panel.getStyle();
            if (!style)
                return;

            style.setOpacity(FootstepSound::getOpacity(sound.getTimeAlive(params.globalVarsProvider.getGlobalVars()->curtime))); 

            cs2::CTransform3D* transformations[]{ params.transformFactory.create<cs2::CTransformScale3D>(
                FootstepSound::getScale(soundInClipSpace.z), FootstepSound::getScale(soundInClipSpace.z), 1.0f
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
        soundWatcher.startWatching<FootstepSound>();
    }

    void onDisable() noexcept
    {
        viewRenderHook.decrementReferenceCount();
        soundWatcher.stopWatching<FootstepSound>();
        panels.hidePanels(0);
    }

    FootstepPanels panels;
    ViewRenderHook& viewRenderHook;
    SoundWatcher& soundWatcher;
};
