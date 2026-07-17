#pragma once

#include <Features/Common/InWorldPanels.h>
#include <Features/Common/FeatureToggle.h>
#include <GameClient/WorldToScreen/WorldToClipSpaceConverter.h>
#include <GameClient/WorldToScreen/ViewToProjectionMatrix.h>
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
#include <Features/Visuals/GrenadePrediction/TrajectoryLineSegment.h>
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
        const auto frametime = hookContext.globalVars().frametime();
        bool shouldUpdate = state.updateScheduler.shouldUpdate(false, frametime.hasValue(), frametime.valueOr(0.0f));

        // Consume a due deadline before observing a possible automatic weapon
        // switch. Its identity is scalar-only and is never dereferenced.
        const void* eventWeapon = state.throwObservation.pendingWeapon();
        auto eventSequence = state.throwObservation.pendingSequence();
        bool event = state.throwObservation.consumeActualExecution(hasCurtime, currentCurtime);
        bool eventStrengthValid = event && state.throwObservation.canCommitActualExecution();
        bool candidate = event && eventStrengthValid
            && state.stageOwnedTempTrajectory(eventWeapon, eventSequence);
        auto finishEvent = [&]() noexcept {
            state.finalizeStagedTrajectory(candidate, hasCurtime, currentCurtime);
            hideLivePrediction();
            renderLastCommittedTrajectory(hasCurtime, currentCurtime);
        };

        // Detect grenade type from active weapon.
        auto weaponPtr = static_cast<cs2::C_BaseEntity*>(activeWeapon.baseEntity());
        if (!weaponPtr) {
            state.throwObservation.reset();
            state.invalidateForSequenceChange();
            if (event) {
                finishEvent();
                return;
            }
            hideLivePrediction();
            if (shouldUpdate)
                renderLastCommittedTrajectory(hasCurtime, currentCurtime);
            return;
        }

        const auto kind = activeWeapon.grenadeKind();
        if (!kind.hasValue()) {
            state.throwObservation.reset();
            state.invalidateForSequenceChange();
            if (event) {
                finishEvent();
                return;
            }
            hideLivePrediction();
            if (shouldUpdate)
                renderLastCommittedTrajectory(hasCurtime, currentCurtime);
            return;
        }

        if (state.throwObservation.observeWeapon(weaponPtr)) {
            hideLivePrediction();
            state.invalidateForSequenceChange();
        }
        const auto throwTimeOpt = hookContext.patternSearchResults().template get<OffsetToThrowTime>()
            .of(static_cast<cs2::C_BaseCSGrenade*>(weaponPtr)).toOptional();
        const auto pinPulledOpt = hookContext.patternSearchResults().template get<OffsetToPinPulled>()
            .of(static_cast<cs2::C_CSWeaponBase*>(weaponPtr)).toOptional();
        const auto sequenceBeforePin = state.throwObservation.sequence;
        const bool releaseEdge = pinPulledOpt.hasValue() && state.throwObservation.observePinState(weaponPtr, pinPulledOpt.value());
        if (sequenceBeforePin != state.throwObservation.sequence) {
            hideLivePrediction();
            state.invalidateForSequenceChange();
        }
        const bool retainStrength = !state.throwObservation.isStrengthLocked()
            && ((pinPulledOpt.hasValue() && pinPulledOpt.value())
                || (throwTimeOpt.hasValue() && throwTimeOpt.value() > 0.0f));
        if (retainStrength) {
            const auto throwStrengthOpt = hookContext.patternSearchResults().template get<OffsetToThrowStrength>()
                .of(static_cast<cs2::C_CSWeaponBase*>(weaponPtr)).toOptional();
            if (throwStrengthOpt.hasValue())
                state.throwObservation.retainThrowStrength(throwStrengthOpt.value());
        }
        // A resolved throw-time source is authoritative, including when curtime
        // is transiently unavailable. Pin release remains solely a legacy fallback.
        const bool scheduledTransition = throwTimeOpt.hasValue() && state.throwObservation.observeThrowTime(weaponPtr, throwTimeOpt.value());
        const bool legacyCycle = !throwTimeOpt.hasValue() && (pinPulledOpt.valueOr(false) || releaseEdge);
        // Capture after throw-time observation establishes or cancels ownership,
        // but before a newly observed deadline is consumed. A prior-hook event
        // may only retain this hook's stash when it belongs to this exact owner.
        if (GrenadeLaunchSnapshot::shouldCaptureForLifecycle(event, eventWeapon, eventSequence, weaponPtr, state.throwObservation.sequence,
                state.throwObservation.hasPendingExecution(), legacyCycle)) {
            // The wrapper resolves movement services once per hook so these five
            // fields are a coherent service snapshot.
            auto&& movementServices = playerPawn.movementServices();
            const auto stashTime = movementServices.stashGrenadeParameterWhen();
            const auto parametersStashed = movementServices.grenadeParametersStashed();
            const auto stashedShootAngles = movementServices.stashedShootAngles();
            const auto stashedThrowPosition = movementServices.stashedGrenadeThrowPosition();
            const auto stashedVelocity = movementServices.stashedVelocity();
            if (stashTime.hasValue() && parametersStashed.hasValue() && stashedShootAngles.hasValue()
                && stashedThrowPosition.hasValue() && stashedVelocity.hasValue()
                && GrenadeLaunchSnapshot::isFresh(parametersStashed.value(), hasCurtime, currentCurtime, stashTime.value())) {
                state.captureLaunchSnapshot(stashTime.value(), stashedShootAngles.value(), stashedThrowPosition.value(), stashedVelocity.value(),
                    weaponPtr, state.throwObservation.sequence);
            }
        }
        const void* newlyExecutingWeapon = state.throwObservation.pendingWeapon();
        const auto newlyExecutingSequence = state.throwObservation.pendingSequence();
        const bool observedActualExecution = state.throwObservation.consumeActualExecution(hasCurtime, currentCurtime);
        if (observedActualExecution) {
            eventWeapon = newlyExecutingWeapon;
            eventSequence = newlyExecutingSequence;
            event = true;
            eventStrengthValid = state.throwObservation.canCommitActualExecution();
            candidate = eventStrengthValid
                && state.stageOwnedTempTrajectory(eventWeapon, eventSequence);
        }
        const float throwStrength = state.throwObservation.retainedThrowStrength;
        const bool legacyEligible = !throwTimeOpt.hasValue() && state.throwObservation.canCommitRelease(releaseEdge);
        const bool legacyRelease = !throwTimeOpt.hasValue() && state.throwObservation.consumeLegacyRelease(releaseEdge);
        if (legacyRelease) {
            eventWeapon = weaponPtr;
            eventSequence = state.throwObservation.sequence;
            event = true;
            eventStrengthValid = legacyEligible;
            candidate = legacyEligible
                && state.stageOwnedTempTrajectory(eventWeapon, eventSequence);
        }
        if (scheduledTransition || event)
            shouldUpdate = state.updateScheduler.shouldUpdate(true, frametime.hasValue(), frametime.valueOr(0.0f));
        if (!shouldUpdate) {
            if (event)
                finishEvent();
            return;
        }

        if (event && eventWeapon != weaponPtr) {
            finishEvent();
            return;
        }
        if (event && !eventStrengthValid) {
            finishEvent();
            return;
        }
        if (!event && state.throwObservation.isFinalized()) {
            hideLivePrediction();
            renderLastCommittedTrajectory(hasCurtime, currentCurtime);
            return;
        }

        const bool ownsLaunchSnapshot = state.ownsLaunchSnapshot(weaponPtr, state.throwObservation.sequence);
        cs2::Vector eyePos{};
        cs2::Vector viewAngles{};
        if (ownsLaunchSnapshot) {
            eyePos = state.launchSnapshot.throwPosition;
            viewAngles = state.launchSnapshot.shootAngles;
        } else {
            auto eyeAngles = playerPawn.eyeAngles();
            if (!eyeAngles.hasValue()) {
                if (event) finishEvent(); else { hideLivePrediction(state.throwObservation.hasPendingExecution()); renderLastCommittedTrajectory(hasCurtime, currentCurtime); }
                return;
            }

            auto absOrigin = playerPawn.absOrigin();
            if (!absOrigin.hasValue()) {
                if (event) finishEvent(); else { hideLivePrediction(state.throwObservation.hasPendingExecution()); renderLastCommittedTrajectory(hasCurtime, currentCurtime); }
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
            eyePos = absOrigin.value() + cs2::Vector{0.0f, 0.0f, eyeHeight};
            viewAngles = eyeAngles.value();
        }

        float baseVelocity = grenade_prediction_params::kBaseThrowVelocity;

        // Get player entity to skip self-collision in traces
        auto playerEntity = static_cast<cs2::C_BaseEntity*>(playerPawn.baseEntity());

        auto simulator = hookContext.template make<GrenadeSimulator<HookContext>>();

        // Compute spawn position using hull trace (IDA verified: trace 22u forward, pull back 6u)
        // Pitch correction and Z offset are handled internally by computeSpawnPosition
        auto spawnPos = simulator.computeSpawnPosition(eyePos, viewAngles, throwStrength, static_cast<void*>(playerEntity));
        if (!spawnPos.hasValue()) {
            if (event) finishEvent(); else { hideLivePrediction(state.throwObservation.hasPendingExecution()); renderLastCommittedTrajectory(hasCurtime, currentCurtime); }
            return;
        }

        // Compute initial velocity (pitch correction is handled internally)
        cs2::Vector initialVelocity = GrenadeSimulator<HookContext>::computeInitialVelocity(viewAngles, baseVelocity, throwStrength);

        // Add player momentum: grenade inherits player velocity scaled
        if (ownsLaunchSnapshot) {
            const auto& velocity = state.launchSnapshot.velocity;
            initialVelocity.x += velocity.x * grenade_prediction_params::kPlayerVelocityScale;
            initialVelocity.y += velocity.y * grenade_prediction_params::kPlayerVelocityScale;
            initialVelocity.z += velocity.z * grenade_prediction_params::kPlayerVelocityScale;
        } else {
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
            state.invalidateTempTrajectory();
            if (event) finishEvent(); else { hideLivePrediction(state.throwObservation.hasPendingExecution()); renderLastCommittedTrajectory(hasCurtime, currentCurtime); }
            return;
        }

        state.tagTempTrajectory(weaponPtr, state.throwObservation.sequence);
        if (event) {
            if (eventWeapon == weaponPtr && eventSequence == state.throwObservation.sequence)
                candidate = state.stageOwnedTempTrajectory(eventWeapon, eventSequence);
            finishEvent();
            return;
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
        state.updateScheduler.reset();
        invalidateTrajectory(state.lastCommittedTrajectory);
        state.invalidateTempTrajectory();
        state.invalidateLaunchSnapshot();
        hideContainerPanel(state.liveContainerPanelHandle);
        hideContainerPanel(state.lastCacheContainerPanelHandle);
    }

private:

    void hideLivePrediction(bool preserveSnapshot = false) noexcept
    {
        auto& state = context().state();
        hideContainerPanel(state.liveContainerPanelHandle);
        if (!preserveSnapshot)
            state.invalidateTempTrajectory();
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
            if (state.rollbackDetected) {
                state.rollbackDetected = false;
                hideLivePrediction();
            }
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
        if (!traj.valid || traj.pointsCount <= 1) {
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

        int segmentCount = traj.pointsCount > 1 ? traj.pointsCount - 1 : 0;
        // +1 for the end-position marker (orange)
        int neededPanels = segmentCount + traj.bouncesCount + (traj.validLanding ? 1 : 0);

        // Create missing dot/line panels
        bool panelsCreated = false;
        while (childCount < neededPanels) {
            auto&& dotPanel = hookContext.panelFactory().createPanel(containerPanel).uiPanel();
            // Origin at center so we can rotate around it
            dotPanel.setTransformOrigin(cs2::CUILength::percent(50.0f), cs2::CUILength::percent(50.0f));
            dotPanel.setVisible(false);
            childCount++;
            panelsCreated = true;
        }

        // Re-fetch children after creation
        auto children = containerPanel.children();
        auto converter = hookContext.template make<WorldToClipSpaceConverter>();
        auto viewToProjectionMatrix = hookContext.template make<ViewToProjectionMatrix<HookContext>>();
        float aspectRatio = viewToProjectionMatrix.getAspectRatio();
        int index = 0;

        auto trajHue = context().config().template getVariable<grenade_prediction_vars::TrajectoryHue>();
        const auto trajectoryHue = static_cast<color::HueInteger>(trajHue).toHueFloat();
        auto trajColor = color::HSBtoRGB(trajectoryHue, color::Saturation{1.0f}, color::Brightness{1.0f}).setAlpha(255);
        auto bounceHue = context().config().template getVariable<grenade_prediction_vars::BounceHue>();
        const auto bounceHueFloat = static_cast<color::HueInteger>(bounceHue).toHueFloat();
        auto bounceColor = color::HSBtoRGB(bounceHueFloat, color::Saturation{1.0f}, color::Brightness{1.0f}).setAlpha(255);

        auto& state = context().state();
        auto& panelStyle = &containerPanelHandle == &state.liveContainerPanelHandle ? state.livePanelStyle : state.lastCachePanelStyle;
        const float trajectoryHueValue = static_cast<float>(trajectoryHue);
        const float bounceHueValue = static_cast<float>(bounceHueFloat);
        const bool updateStyles = panelsCreated || !panelStyle.initialized || panelStyle.pointsCount != segmentCount
            || panelStyle.bouncesCount != traj.bouncesCount || panelStyle.validLanding != traj.validLanding
            || panelStyle.trajectoryHue != trajectoryHueValue || panelStyle.bounceHue != bounceHueValue;

        if (updateStyles) {
            constexpr auto kCenteredAlignment = PanelAlignmentParams{
                .horizontalAlignment = cs2::k_EHorizontalAlignmentCenter,
                .verticalAlignment = cs2::k_EVerticalAlignmentCenter};
            for (int i = 0; i < neededPanels; ++i)
                children[i].setAlign(kCenteredAlignment);
        }

        // Orange color for detonation/landing marker
        auto endMarkerColor = color::HSBtoRGB(grenade_prediction_params::kEndMarkerHue, color::Saturation{1.0f}, color::Brightness{1.0f}).setAlpha(255);

        // Draw trajectory segments
        for (int i = 0; i < segmentCount; ++i) {
            const auto& pt1 = traj.points[i];
            const auto& pt2 = traj.points[i + 1];
            auto clipSpace1 = converter.toClipSpace(pt1);
            auto clipSpace2 = converter.toClipSpace(pt2);

            auto child = children[index];
            if (updateStyles) {
                child.setHeight(cs2::CUILength::pixels(grenade_prediction_params::kTrajectoryLineThickness));
                child.setBackgroundColor(trajColor);
            }

            TrajectoryLineSegment segment;
            if (TrajectoryLineSegment::fromClipSpace(clipSpace1, clipSpace2, aspectRatio, segment)) {
                child.setWidth(cs2::CUILength::percent(segment.width));
                child.setRotate2dCentered(segment.angleDegrees);
                child.setVisible(true);
                PanoramaTransformations{
                    hookContext.panoramaTransformFactory().translate(
                        cs2::CUILength::percent(segment.midpointX - 50.0f),
                        cs2::CUILength::percent(segment.midpointY - 50.0f))
                }.applyTo(child);
            } else {
                child.setVisible(false);
            }
            index++;
        }

        // Draw bounce points (larger squares)
        for (int i = 0; i < traj.bouncesCount; ++i) {
            const auto& pt = traj.bounces[i];
            auto clipSpace = converter.toClipSpace(pt);
            auto child = children[index];
            if (updateStyles) {
                child.setWidth(cs2::CUILength::pixels(grenade_prediction_params::kBounceDotSize));
                child.setHeight(cs2::CUILength::pixels(grenade_prediction_params::kBounceDotSize));
                child.setRotate2dCentered(0.0f);
                child.setBackgroundColor(bounceColor);
            }

            bool visible = false;
            if (clipSpace.onScreen()) {
                float w = clipSpace.w;
                if (clipSpace.x >= -w && clipSpace.x <= w &&
                    clipSpace.y >= -w && clipSpace.y <= w) {
                    auto deviceCoords = clipSpace.toNormalizedDeviceCoordinates();
                    visible = true;
                    child.setVisible(true);
                    PanoramaTransformations{
                        hookContext.panoramaTransformFactory().translate(cs2::CUILength::percent(deviceCoords.getX().m_flValue - 50.0f), cs2::CUILength::percent(deviceCoords.getY().m_flValue - 50.0f))
                    }.applyTo(child);
                }
            }
            if (!visible)
                child.setVisible(false);
            index++;
        }

        // Draw end-position marker (orange, 10x10 square)
        if (traj.validLanding)
        {
            auto clipSpace = converter.toClipSpace(traj.endPos);
            auto child = children[index];
            if (updateStyles) {
                child.setWidth(cs2::CUILength::pixels(grenade_prediction_params::kEndMarkerSize));
                child.setHeight(cs2::CUILength::pixels(grenade_prediction_params::kEndMarkerSize));
                child.setRotate2dCentered(0.0f);
                child.setBackgroundColor(endMarkerColor);
            }

            bool visible = false;
            if (clipSpace.onScreen()) {
                float w = clipSpace.w;
                if (clipSpace.x >= -w && clipSpace.x <= w &&
                    clipSpace.y >= -w && clipSpace.y <= w) {
                    auto deviceCoords = clipSpace.toNormalizedDeviceCoordinates();
                    visible = true;
                    child.setVisible(true);
                    PanoramaTransformations{
                        hookContext.panoramaTransformFactory().translate(cs2::CUILength::percent(deviceCoords.getX().m_flValue - 50.0f), cs2::CUILength::percent(deviceCoords.getY().m_flValue - 50.0f))
                    }.applyTo(child);
                }
            }
            if (!visible)
                child.setVisible(false);
            index++;
        }

        // Only the tail that became unused needs hiding.
        int& previousActiveCount = &containerPanelHandle == &state.liveContainerPanelHandle
            ? state.liveActivePanelCount : state.lastCacheActivePanelCount;
        while (index < previousActiveCount && index < childCount) {
            children[index].setVisible(false);
            index++;
        }
        previousActiveCount = neededPanels;
        panelStyle = {segmentCount, traj.bouncesCount, traj.validLanding, trajectoryHueValue, bounceHueValue, true};
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
