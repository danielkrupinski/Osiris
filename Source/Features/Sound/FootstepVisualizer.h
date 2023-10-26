#pragma once

#include <algorithm>
#include <array>

#include <CS2/Classes/Panorama.h>
#include <CS2/Constants/SoundNames.h>
#include <FeatureHelpers/PlayedSound.h>
#include <FeatureHelpers/PlayedSounds.h>
#include <FeatureHelpers/TogglableFeature.h>
#include <FeatureHelpers/WorldToClipSpaceConverter.h>
#include <GameClasses/PanoramaUiEngine.h>
#include <Helpers/HudProvider.h>
#include <Helpers/PanoramaTransformFactory.h>
#include <Hooks/ViewRenderHook.h>

class FootstepVisualizer : public TogglableFeature<FootstepVisualizer> {
public:
    explicit FootstepVisualizer(HudProvider hudProvider, ViewRenderHook& viewRenderHook) noexcept
        : hudProvider{ hudProvider }
        , viewRenderHook{ viewRenderHook }
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

        if (!globalVars || !*globalVars)
            return;

        if (!worldtoClipSpaceConverter)
            return;

        createFootstepPanels();
        removeOldFootsteps(**globalVars);
        collectFootsteps(**globalVars);

        std::size_t currentIndex = 0;
        footsteps.forEach([this, &currentIndex] (const PlayedSound& sound) {
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

            const auto timeAlive = sound.getTimeAlive((*globalVars)->curtime);
            if (timeAlive >= kFootstepLifespan - kFootstepFadeAwayDuration) {
                style.setOpacity((kFootstepLifespan - timeAlive) / kFootstepFadeAwayDuration);
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

    void onEnable() const noexcept
    {
        viewRenderHook.incrementReferenceCount();
    }

    void onDisable() const noexcept
    {
        viewRenderHook.decrementReferenceCount();
        hidePanels(0);
    }

    void createFootstepPanels() noexcept
    {
        if (footstepContainerPanelPointer.get())
            return;

        const auto hudReticle = hudProvider.findChildInLayoutFile(cs2::HudReticle);
        if (!hudReticle)
            return;

        PanoramaUiEngine::runScript(hudReticle,
            R"(
$.CreatePanel('Panel', $.GetContextPanel(), 'FootstepContainer', {
  style: 'width: 100%; height: 100%;'
});)", "", 0);

        footstepContainerPanelPointer = hudReticle.findChildInLayoutFile("FootstepContainer");

        for (std::size_t i = 0; i < kMaxNumberOfFootstepsToDraw; ++i) {
            PanoramaUiEngine::runScript(hudReticle,
                R"(
(function() {
var footstepPanel = $.CreatePanel('Panel', $.GetContextPanel().FindChildInLayoutFile("FootstepContainer"), '', {
  style: 'width: 50px; height: 50px; x: -25px; y: -25px;'
});

$.CreatePanel('Image', footstepPanel, '', {
  src: "s2r://panorama/images/icons/equipment/stomp_damage.svg",
  style: "horizontal-align: center; vertical-align: center; img-shadow: 0px 0px 1px 3 #000000;"
});
})();)", "", 0);
        }
    }

    void removeOldFootsteps(const cs2::GlobalVars& globalVars) noexcept
    {
        footsteps.removeExpiredSounds(globalVars.curtime, kFootstepLifespan);
    }

    void collectFootsteps(const cs2::GlobalVars& globalVars) noexcept
    {
        footsteps.collectSounds(globalVars.curtime, [](std::string_view soundName) {
            if (soundName.starts_with(cs2::kPlayerFootstepSoundsPath)) {
                return !std::string_view{ soundName.data() + cs2::kPlayerFootstepSoundsPath.length(), soundName.length() - cs2::kPlayerFootstepSoundsPath.length() }.starts_with(cs2::kPlayerSuitSoundPrefix);
            }
            return false;
        });
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
    cs2::GlobalVars** globalVars{ ClientPatterns::globalVars() };
    PanoramaTransformFactory transformFactory;
    ViewRenderHook& viewRenderHook;

    WorldToClipSpaceConverter worldtoClipSpaceConverter;
    PlayedSounds footsteps;

    static constexpr auto kMaxNumberOfFootstepsToDraw = 100;
    static constexpr auto kFootstepLifespan = 2.0f;
    static constexpr auto kFootstepFadeAwayDuration = 0.4f;
};
