#pragma once

#include <Features/Common/InWorldPanels.h>
#include <Features/Common/FeatureToggle.h>
#include <GameClient/WorldToScreen/WorldToClipSpaceConverter.h>
#include <MemoryPatterns/PatternTypes/GlobalVarsPatternTypes.h>
#include <GameClient/GlobalVars.h>
#include <CS2/Classes/Entities/C_CSPlayerPawn.h>
#include <CS2/Classes/Entities/WeaponEntities.h>
#include <CS2/Classes/Entities/C_CSWeaponBase.h>
#include <CS2/Panorama/CUIEngine.h>
#include <GameClient/Panorama/PanelHandle.h>
#include <GameClient/Panorama/PanoramaUiEngine.h>
#include <GameClient/Panorama/PanoramaTransformations.h>
#include <MemoryPatterns/PatternTypes/WeaponPatternTypes.h>
#include <cstring>
#include <cstdint>
#include <Utils/Lvalue.h>

#include <Features/Visuals/GrenadePrediction/GrenadePredictionContext.h>
#include <Features/Visuals/GrenadePrediction/GrenadePredictionState.h>
#include <Features/Visuals/GrenadePrediction/GrenadeSimulator.h>
#include <Features/Visuals/GrenadePrediction/GrenadePredictionConfigVariables.h>
#include <Features/Visuals/GrenadePrediction/GrenadePredictionParams.h>
#include <Utils/ColorUtils.h>

template <typename HookContext>
class GrenadePrediction {
public:
    explicit GrenadePrediction(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void handleGrenadePrediction(auto&& playerPawn, auto&& activeWeapon) noexcept
    {
        if (!playerPawn.isControlledByLocalPlayer()) {
            return;
        }

        const auto alive = playerPawn.isAlive();
        if (!alive.has_value() || !*alive) {
            clearPrediction();
            return;
        }

        if (!context().condition().shouldRun()) {
            clearPrediction();
            return;
        }

        auto& state = context().state();
        const auto curtime = hookContext.globalVars().curtime();
        const bool hasCurtime = curtime.hasValue();
        const float currentCurtime = hasCurtime ? curtime.value() : 0.0f;
        updateCacheForTime(hasCurtime, currentCurtime);

        // Detect grenade type from active weapon.
        auto weaponPtr = static_cast<cs2::C_BaseEntity*>(activeWeapon.baseEntity());
        if (!weaponPtr) {
            state.throwObservation.reset();
            hideLivePrediction();
            renderLastCommittedTrajectory(hasCurtime, currentCurtime);
            return;
        }

        const auto kind = activeWeapon.grenadeKind();
        if (!kind.hasValue()) {
            state.throwObservation.reset();
            hideLivePrediction();
            renderLastCommittedTrajectory(hasCurtime, currentCurtime);
            return;
        }

        // Consume the pin edge before any simulation inputs are evaluated, so a
        // failed release frame cannot be retried on subsequent pin=false frames.
        const auto pinPulledOpt = hookContext.patternSearchResults().template get<OffsetToPinPulled>()
            .of(static_cast<cs2::C_CSWeaponBase*>(weaponPtr)).toOptional();
        if (!pinPulledOpt.hasValue()) {
            state.throwObservation.reset();
            hideLivePrediction();
            renderLastCommittedTrajectory(hasCurtime, currentCurtime);
            return;
        }
        const bool releaseEdge = state.throwObservation.observePinState(weaponPtr, pinPulledOpt.value());
        if (pinPulledOpt.value()) {
            const auto throwStrengthOpt = hookContext.patternSearchResults().template get<OffsetToThrowStrength>()
                .of(static_cast<cs2::C_CSWeaponBase*>(weaponPtr)).toOptional();
            if (throwStrengthOpt.hasValue())
                state.throwObservation.retainThrowStrength(throwStrengthOpt.value());
        }
        const float throwStrength = state.throwObservation.retainedThrowStrength;

        auto eyeAngles = playerPawn.eyeAngles();
        if (!eyeAngles.hasValue()) {
            hideLivePrediction();
            renderLastCommittedTrajectory(hasCurtime, currentCurtime);
            return;
        }

        auto absOrigin = playerPawn.absOrigin();
        if (!absOrigin.hasValue()) {
            hideLivePrediction();
            renderLastCommittedTrajectory(hasCurtime, currentCurtime);
            return;
        }

        // Eye position: absOrigin + viewOffset
        // Read m_vecViewOffset (Z component) to handle crouching
        float eyeHeight = grenade_prediction_params::kDefaultEyeHeight;
        {
            auto* pawnEntity = static_cast<cs2::C_BaseEntity*>(playerPawn.baseEntity());
            if (pawnEntity) {
                auto viewOffsetOpt = hookContext.patternSearchResults().template get<OffsetToViewOffset>()
                    .of(pawnEntity).toOptional();
                if (viewOffsetOpt.hasValue()) {
                    float vz = viewOffsetOpt.value().z;
                    if (vz > grenade_prediction_params::kViewOffsetMin && vz < grenade_prediction_params::kViewOffsetMax)
                        eyeHeight = vz;
                }
            }
        }
        cs2::Vector eyePos = absOrigin.value() + cs2::Vector{0.0f, 0.0f, eyeHeight};
        cs2::Vector viewAngles = eyeAngles.value();

        float baseVelocity = grenade_prediction_params::kBaseThrowVelocity;

        // Get player entity to skip self-collision in traces
        auto playerEntity = static_cast<cs2::C_BaseEntity*>(playerPawn.baseEntity());

        auto simulator = hookContext.template make<GrenadeSimulator<HookContext>>();

        // Compute spawn position using hull trace (IDA verified: trace 22u forward, pull back 6u)
        // Pitch correction and Z offset are handled internally by computeSpawnPosition
        auto spawnPos = simulator.computeSpawnPosition(eyePos, viewAngles, throwStrength, static_cast<void*>(playerEntity));
        if (!spawnPos.hasValue()) {
            hideLivePrediction();
            renderLastCommittedTrajectory(hasCurtime, currentCurtime);
            return;
        }

        // Compute initial velocity (pitch correction is handled internally)
        cs2::Vector initialVelocity = GrenadeSimulator<HookContext>::computeInitialVelocity(viewAngles, baseVelocity, throwStrength);

        // Add player momentum: grenade inherits player velocity scaled
        {
            auto playerVelOpt = playerPawn.baseEntity().absVelocity();
            if (playerVelOpt.hasValue()) {
                cs2::Vector pv = playerVelOpt.value();
                initialVelocity.x += pv.x * grenade_prediction_params::kPlayerVelocityScale;
                initialVelocity.y += pv.y * grenade_prediction_params::kPlayerVelocityScale;
                initialVelocity.z += pv.z * grenade_prediction_params::kPlayerVelocityScale;
            }
        }

        auto& traj = state.tempTrajectory;
        simulator.simulate(traj, spawnPos.value(), initialVelocity, kind.value(), static_cast<void*>(playerEntity));

        if (!traj.valid || traj.pointsCount == 0) {
            hideLivePrediction();
            renderLastCommittedTrajectory(hasCurtime, currentCurtime);
            return;
        }

        if (state.throwObservation.canCommitRelease(releaseEdge)) {
            state.lastCommittedTrajectory = traj;
            if (hasCurtime) {
                state.lastCommitCurtime = currentCurtime;
                state.hasCommitCurtime = true;
            } else if (state.hasLastValidCurtime) {
                state.lastCommitCurtime = state.lastValidCurtime;
                state.hasCommitCurtime = true;
            } else {
                state.lastCommitCurtime = 0.0f;
                state.hasCommitCurtime = false;
            }
        }
        drawTrajectory(traj, state.liveContainerPanelHandle);
        renderLastCommittedTrajectory(hasCurtime, currentCurtime);
    }

    void clearPrediction() noexcept
    {
        auto& state = context().state();
        state.throwObservation.reset();
        state.lastCommitCurtime = 0.0f;
        state.lastValidCurtime = 0.0f;
        state.hasCommitCurtime = false;
        state.hasLastValidCurtime = false;
        invalidateTrajectory(state.lastCommittedTrajectory);
        invalidateTrajectory(state.tempTrajectory);
        hideContainerPanel(state.liveContainerPanelHandle);
        hideContainerPanel(state.lastCacheContainerPanelHandle);
    }

private:

    void hideLivePrediction() noexcept
    {
        auto& state = context().state();
        invalidateTrajectory(state.tempTrajectory);
        hideContainerPanel(state.liveContainerPanelHandle);
    }

    static void invalidateTrajectory(Trajectory& trajectory) noexcept
    {
        trajectory.pointsCount = 0;
        trajectory.bouncesCount = 0;
        trajectory.valid = false;
        trajectory.validLanding = false;
    }

    void updateCacheForTime(bool hasCurtime, float curtime) noexcept
    {
        auto& state = context().state();
        const auto alwaysShow = static_cast<bool>(context().config().template getVariable<grenade_prediction_vars::AlwaysShowLastCache>());
        const auto duration = static_cast<float>(context().config().template getVariable<grenade_prediction_vars::CacheDuration>());
        if (state.cacheVisibility(alwaysShow, duration, hasCurtime, curtime) == LastGrenadeCacheVisibility::Invalidate) {
            invalidateTrajectory(state.lastCommittedTrajectory);
            state.hasCommitCurtime = false;
            hideContainerPanel(state.lastCacheContainerPanelHandle);
        }
    }

    void renderLastCommittedTrajectory(bool hasCurtime, float curtime) noexcept
    {
        auto& state = context().state();
        const auto alwaysShow = static_cast<bool>(context().config().template getVariable<grenade_prediction_vars::AlwaysShowLastCache>());
        const auto duration = static_cast<float>(context().config().template getVariable<grenade_prediction_vars::CacheDuration>());
        switch (state.cacheVisibility(alwaysShow, duration, hasCurtime, curtime)) {
        case LastGrenadeCacheVisibility::Show:
            drawTrajectory(state.lastCommittedTrajectory, state.lastCacheContainerPanelHandle);
            break;
        case LastGrenadeCacheVisibility::Invalidate:
            invalidateTrajectory(state.lastCommittedTrajectory);
            state.hasCommitCurtime = false;
            [[fallthrough]];
        case LastGrenadeCacheVisibility::Hide:
            hideContainerPanel(state.lastCacheContainerPanelHandle);
            break;
        }
    }

    void hideContainerPanel(cs2::PanelHandle& containerPanelHandle) noexcept
    {
        auto&& panel = hookContext.template make<PanelHandle>(containerPanelHandle).get();
        if (panel)
            panel.setVisible(false);
    }

    void drawTrajectory(const Trajectory& traj, cs2::PanelHandle& containerPanelHandle) noexcept
    {
        if (!traj.valid || traj.pointsCount == 0) {
            hideContainerPanel(containerPanelHandle);
            return;
        }

        auto&& containerPanel = getContainerPanel(containerPanelHandle);
        if (!containerPanel)
            return;

        containerPanel.setVisible(true);

        // Count existing children
        auto childrenProxy = containerPanel.children();
        int childCount = (childrenProxy.vector && childrenProxy.vector->memory) ? childrenProxy.vector->size : 0;

        // +1 for the end-position marker (orange) — only if landing is valid
        int neededPanels = traj.pointsCount + traj.bouncesCount + (traj.validLanding ? 1 : 0);

        // Create missing dot panels
        while (childCount < neededPanels) {
            auto&& dotPanel = hookContext.panelFactory().createPanel(containerPanel).uiPanel();
            dotPanel.setWidth(cs2::CUILength::pixels(grenade_prediction_params::kDotPanelCreationSize));
            dotPanel.setHeight(cs2::CUILength::pixels(grenade_prediction_params::kDotPanelCreationSize));
            dotPanel.setTransformOrigin(cs2::CUILength::percent(50.0f), cs2::CUILength::percent(50.0f));
            dotPanel.setVisible(false);
            childCount++;
        }

        // Re-fetch children after creation
        auto children = containerPanel.children();
        auto converter = hookContext.template make<WorldToClipSpaceConverter>();
        int index = 0;

        auto trajHue = context().config().template getVariable<grenade_prediction_vars::TrajectoryHue>();
        auto trajColor = color::HSBtoRGB(trajHue, color::Saturation{1.0f}, color::Brightness{1.0f}).setAlpha(255);
        auto bounceHue = context().config().template getVariable<grenade_prediction_vars::BounceHue>();
        auto bounceColor = color::HSBtoRGB(bounceHue, color::Saturation{1.0f}, color::Brightness{1.0f}).setAlpha(255);

        // Orange color for detonation/landing marker
        auto endMarkerColor = color::HSBtoRGB(grenade_prediction_params::kEndMarkerHue, color::Saturation{1.0f}, color::Brightness{1.0f}).setAlpha(255);

        // Draw trajectory points
        for (int i = 0; i < traj.pointsCount; ++i) {
            const auto& pt = traj.points[i];
            auto clipSpace = converter.toClipSpace(pt);
            auto child = children[index];

            // onScreen() checks w >= 0.001 (in front of camera).
            // Additionally check clip space bounds (-w <= x <= w, -w <= y <= w)
            // to hide points that are in front of camera but outside the viewport.
            bool visible = false;
            if (clipSpace.onScreen()) {
                float w = clipSpace.w;
                if (clipSpace.x >= -w && clipSpace.x <= w &&
                    clipSpace.y >= -w && clipSpace.y <= w) {
                    auto deviceCoords = clipSpace.toNormalizedDeviceCoordinates();
                    visible = true;
                    child.setVisible(true);
                    child.setWidth(cs2::CUILength::pixels(grenade_prediction_params::kTrajectoryDotSize));
                    child.setHeight(cs2::CUILength::pixels(grenade_prediction_params::kTrajectoryDotSize));
                    child.setBackgroundColor(trajColor);
                    PanoramaTransformations{
                        hookContext.panoramaTransformFactory().translate(deviceCoords.getX(), deviceCoords.getY())
                    }.applyTo(child);
                }
            }
            if (!visible)
                child.setVisible(false);
            index++;
        }

        // Draw bounce points (larger squares)
        for (int i = 0; i < traj.bouncesCount; ++i) {
            const auto& pt = traj.bounces[i];
            auto clipSpace = converter.toClipSpace(pt);
            auto child = children[index];

            bool visible = false;
            if (clipSpace.onScreen()) {
                float w = clipSpace.w;
                if (clipSpace.x >= -w && clipSpace.x <= w &&
                    clipSpace.y >= -w && clipSpace.y <= w) {
                    auto deviceCoords = clipSpace.toNormalizedDeviceCoordinates();
                    visible = true;
                    child.setVisible(true);
                    child.setWidth(cs2::CUILength::pixels(grenade_prediction_params::kBounceDotSize));
                    child.setHeight(cs2::CUILength::pixels(grenade_prediction_params::kBounceDotSize));
                    child.setBackgroundColor(bounceColor);
                    PanoramaTransformations{
                        hookContext.panoramaTransformFactory().translate(deviceCoords.getX(), deviceCoords.getY())
                    }.applyTo(child);
                }
            }
            if (!visible)
                child.setVisible(false);
            index++;
        }

        // Draw end-position marker (orange, 10x10 square)
        // Suppressed for Molotov/Incendiary when the grenade detonated mid-air.
        if (traj.validLanding)
        {
            auto clipSpace = converter.toClipSpace(traj.endPos);
            auto child = children[index];

            bool visible = false;
            if (clipSpace.onScreen()) {
                float w = clipSpace.w;
                if (clipSpace.x >= -w && clipSpace.x <= w &&
                    clipSpace.y >= -w && clipSpace.y <= w) {
                    auto deviceCoords = clipSpace.toNormalizedDeviceCoordinates();
                    visible = true;
                    child.setVisible(true);
                    child.setWidth(cs2::CUILength::pixels(grenade_prediction_params::kEndMarkerSize));
                    child.setHeight(cs2::CUILength::pixels(grenade_prediction_params::kEndMarkerSize));
                    child.setBackgroundColor(endMarkerColor);
                    PanoramaTransformations{
                        hookContext.panoramaTransformFactory().translate(deviceCoords.getX(), deviceCoords.getY())
                    }.applyTo(child);
                }
            }
            if (!visible)
                child.setVisible(false);
            index++;
        }

        // Hide remaining unused panels
        int totalChildren = (children.vector && children.vector->memory) ? children.vector->size : 0;
        while (index < totalChildren) {
            children[index].setVisible(false);
            index++;
        }
    }

    [[nodiscard]] decltype(auto) getContainerPanel(cs2::PanelHandle& containerPanelHandle) const noexcept
    {
        return hookContext.template make<PanelHandle>(containerPanelHandle)
            .getOrInit(createContainerPanel());
    }

    [[nodiscard]] auto createContainerPanel() const noexcept
    {
        return [this]() -> decltype(auto) {
            auto&& panel = hookContext.panelFactory().createPanel(hookContext.hud().getHudReticle()).uiPanel();
            panel.fitParent();
            return utils::lvalue<decltype(panel)>(panel);
        };
    }

    [[nodiscard]] decltype(auto) context() const noexcept
    {
        return hookContext.template make<GrenadePredictionContext>();
    }

    HookContext& hookContext;

    template <typename>
    friend struct GrenadePredictionTestAccess;
};
