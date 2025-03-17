#pragma once

#include <utility>

#include <GameClient/Panorama/PanoramaTransformations.h>
#include <Features/Sound/Details/PlayedSound.h>
#include <Features/Sound/Details/SoundWatcher.h>
#include <Features/Common/FeatureToggle.h>
#include <GameClient/WorldToScreen/ViewToProjectionMatrix.h>
#include <GameClient/WorldToScreen/WorldToClipSpaceConverter.h>
#include <Features/Common/InWorldPanels.h>
#include <Hooks/ViewRenderHook.h>
#include <MemoryPatterns/PatternTypes/ClientPatternTypes.h>
#include "SoundVisualization.h"

template <typename HookContext, typename PanelsType, typename SoundType>
class SoundVisualizationFeature {
public:
    SoundVisualizationFeature(
        HookContext& hookContext,
        ViewRenderHook& viewRenderHook,
        SoundWatcher<HookContext> soundWatcher) noexcept
        : hookContext{hookContext}
        , viewRenderHook{viewRenderHook}
        , soundWatcher{soundWatcher}
    {
    }

    void run() noexcept
    {
        if (!enabled())
            return;

        const auto curtime = hookContext.globalVars().curtime();
        if (!curtime.hasValue())
            return;

        std::as_const(soundWatcher).template getSoundsOfType<SoundType>().forEach([this, curtime](const PlayedSound& sound) {
            const auto soundInClipSpace = hookContext.template make<WorldToClipSpaceConverter>().toClipSpace(sound.origin);
            if (!soundInClipSpace.onScreen())
                return;

            const auto opacity = SoundVisualization<SoundType>::getOpacity(sound.getTimeAlive(curtime.value()));
            if (opacity <= 0.0f)
                return;

            auto&& panel = hookContext.template make<InWorldPanels>().template getNextSoundVisualizationPanel<PanelsType>();
            panel.setOpacity(opacity);
            panel.setZIndex(-soundInClipSpace.z);

            const auto deviceCoordinates = soundInClipSpace.toNormalizedDeviceCoordinates();

            auto&& transformFactory = hookContext.panoramaTransformFactory();
            PanoramaTransformations{
                transformFactory.scale(SoundVisualization<SoundType>::getScale(soundInClipSpace.z, ViewToProjectionMatrix{hookContext.clientPatternSearchResults().template get<ViewToProjectionMatrixPointer>()}.getFovScale())),
                transformFactory.translate(deviceCoordinates.getX(), deviceCoordinates.getY())
            }.applyTo(panel);
        });
    }

private:
    [[nodiscard]] auto enabled() noexcept
    {
        if constexpr (std::is_same_v<SoundType, BombBeepSound>)
            return hookContext.config().template getVariable<BombBeepSoundVisualizationEnabled>();
        else if constexpr (std::is_same_v<SoundType, BombDefuseSound>)
            return hookContext.config().template getVariable<BombBeepSoundVisualizationEnabled>();
        else if constexpr (std::is_same_v<SoundType, BombPlantSound>)
            return hookContext.config().template getVariable<BombPlantSoundVisualizationEnabled>();
        else if constexpr (std::is_same_v<SoundType, FootstepSound>)
            return hookContext.config().template getVariable<FootstepSoundVisualizationEnabled>();
        else if constexpr (std::is_same_v<SoundType, WeaponReloadSound>)
            return hookContext.config().template getVariable<WeaponReloadSoundVisualizationEnabled>();
        else if constexpr (std::is_same_v<SoundType, WeaponScopeSound>)
            return hookContext.config().template getVariable<WeaponScopeSoundVisualizationEnabled>();
        else
            static_assert(!std::is_same_v<SoundType, SoundType>, "Unknown type");
    }

    HookContext& hookContext;
    ViewRenderHook& viewRenderHook;
    SoundWatcher<HookContext> soundWatcher;
};
