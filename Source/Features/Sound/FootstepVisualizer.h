#pragma once

#include <algorithm>
#include <array>

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/SoundNames.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/WorldToClipSpaceConverter.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaTransformFactory.h>
#include <Hooks/ViewRenderHook.h>

class FootstepVisualizer : public TogglableFeature<FootstepVisualizer> {
public:
    explicit FootstepVisualizer(HudProvider hudProvider, GlobalVarsProvider globalVarsProvider, ViewRenderHook& viewRenderHook, SoundWatcher& soundWatcher) noexcept
        : hudProvider{ hudProvider }
        , globalVarsProvider{ globalVarsProvider }
        , viewRenderHook{ viewRenderHook }
        , soundWatcher{ soundWatcher }
    {
    }

    ~FootstepVisualizer() noexcept
    {
        if (footstepContainerPanelPointer.getHandle().isValid())
            PanoramaUiEngine::onDeletePanel(footstepContainerPanelPointer.getHandle());
    }

    void run() noexcept
    {
        if (!isEnabled())
            return;

        if (!globalVarsProvider || !globalVarsProvider.getGlobalVars())
            return;

        if (!worldtoClipSpaceConverter)
            return;

        createFootstepPanels();

        std::size_t currentIndex = 0;
        soundWatcher.forEach<WatchedSoundType::Footsteps>([this, &currentIndex] (const PlayedSound& sound) {
            const auto soundInClipSpace = worldtoClipSpaceConverter.toClipSpace(sound.origin);
            if (!soundInClipSpace.onScreen())
                return;
            
            const auto deviceCoordinates = soundInClipSpace.toNormalizedDeviceCoordinates();
            const auto panel = getFootstepPanel(currentIndex);
            if (!panel)
                return;

            const auto style = panel.getStyle();
            if (!style)
                return;

            const auto timeAlive = sound.getTimeAlive(globalVarsProvider.getGlobalVars()->curtime);
            if (timeAlive >= SoundWatcher::kFootstepLifespan - kFootstepFadeAwayDuration) {
                style.setOpacity((SoundWatcher::kFootstepLifespan - timeAlive) / kFootstepFadeAwayDuration);
            } else {
                style.setOpacity(1.0f);
            }

            cs2::CTransform3D* transformations[]{ transformFactory.create<cs2::CTransformScale3D>(
                (std::max)(1.0f - soundInClipSpace.z / 1000.0f, 0.3f), (std::max)(1.0f - soundInClipSpace.z / 1000.0f, 0.3f), 1.0f
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
        soundWatcher.startWatching<WatchedSoundType::Footsteps>();
    }

    void onDisable() noexcept
    {
        viewRenderHook.decrementReferenceCount();
        soundWatcher.stopWatching<WatchedSoundType::Footsteps>();
        hidePanels(0);
    }

    void createFootstepPanels() noexcept
    {
        if (footstepContainerPanelPointer.get())
            return;

        const auto hudReticle = hudProvider.findChildInLayoutFile(cs2::HudReticle);
        if (!hudReticle)
            return;

        const auto footstepContainer = Panel::create("FootstepContainer", hudReticle);
        if (!footstepContainer)
            return;

        footstepContainerPanelPointer = footstepContainer->uiPanel;

        if (const auto style = PanoramaUiPanel{ footstepContainer->uiPanel }.getStyle()) {
            style.setWidth(cs2::CUILength{ 100.0f, cs2::CUILength::k_EUILengthPercent });
            style.setHeight(cs2::CUILength{ 100.0f, cs2::CUILength::k_EUILengthPercent });
        }

        for (std::size_t i = 0; i < kMaxNumberOfFootstepsToDraw; ++i) {
            PanoramaUiEngine::runScript(hudReticle,
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

    [[nodiscard]] PanoramaUiPanel getFootstepPanel(std::size_t index) const noexcept
    {
        const auto footstepContainerPanel = footstepContainerPanelPointer.get();
        if (!footstepContainerPanel)
            return PanoramaUiPanel{nullptr};

        if (const auto children = footstepContainerPanel.children()) {
            if (children->size > 0 && static_cast<std::size_t>(children->size) > index)
                return PanoramaUiPanel{ children->memory[index] };
        }
        return PanoramaUiPanel{nullptr};
    }

    void hidePanels(std::size_t fromPanelIndex) const noexcept
    {
        for (std::size_t i = fromPanelIndex; i < kMaxNumberOfFootstepsToDraw; ++i) {
            const auto panel = getFootstepPanel(i);
            if (!panel)
                break;

            if (const auto style = panel.getStyle())
                style.setOpacity(0.0f);
        }
    }

    HudProvider hudProvider;
    PanoramaPanelPointer footstepContainerPanelPointer;
    GlobalVarsProvider globalVarsProvider;
    PanoramaTransformFactory transformFactory;
    ViewRenderHook& viewRenderHook;

    WorldToClipSpaceConverter worldtoClipSpaceConverter;
    SoundWatcher& soundWatcher;

    static constexpr auto kMaxNumberOfFootstepsToDraw = 100;
    static constexpr auto kFootstepFadeAwayDuration = 0.4f;
};
