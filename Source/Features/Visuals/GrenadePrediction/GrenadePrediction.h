#pragma once

#include <Features/Common/InWorldPanels.h>
#include <Features/Common/FeatureToggle.h>
#include <GameClient/WorldToScreen/WorldToClipSpaceConverter.h>
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

        if (!context().condition().shouldRun()) {
            clearPrediction();
            return;
        }

        // Detect grenade type from active weapon
        auto weaponPtr = static_cast<cs2::C_BaseEntity*>(activeWeapon.baseEntity());
        if (!weaponPtr) {
            clearPrediction();
            return;
        }

        auto& state = context().state();
        if (state.cachedThrowStrengthWeapon != weaponPtr) {
            state.cachedThrowStrengthWeapon = weaponPtr;
            state.cachedThrowStrength = 1.0f;
        }

        const auto kind = activeWeapon.grenadeKind();
        if (!kind.hasValue()) {
            if (activeWeapon.getName()) {
                state.cachedThrowStrength = 1.0f;
                state.cachedThrowStrengthWeapon = nullptr;
                drawTrajectory(context().state().cachedTrajectory);
            } else {
                clearPrediction();
            }
            return;
        }


        auto eyeAngles = playerPawn.eyeAngles();
        if (!eyeAngles.hasValue()) {
            hideCurrentPrediction();
            return;
        }

        auto absOrigin = playerPawn.absOrigin();
        if (!absOrigin.hasValue()) {
            hideCurrentPrediction();
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

        // Read throw strength from weapon entity (pattern-based offset)
        // Cache the value while pin is pulled. When pin is released (throw animation),
        // m_bPinPulled goes false instantly but the grenade hasn't spawned yet —
        // use the cached value to prevent the prediction from flickering to 1.0f.
        float throwStrength = state.cachedThrowStrength;
        {
            auto pinPulledOpt = hookContext.patternSearchResults().template get<OffsetToPinPulled>()
                .of(static_cast<cs2::C_CSWeaponBase*>(weaponPtr)).toOptional();

            if (pinPulledOpt.hasValue() && pinPulledOpt.value()) {
                auto throwStrengthOpt = hookContext.patternSearchResults().template get<OffsetToThrowStrength>()
                    .of(static_cast<cs2::C_CSWeaponBase*>(weaponPtr)).toOptional();
                if (throwStrengthOpt.hasValue()) {
                    float ts = throwStrengthOpt.value();
                    if (ts >= 0.0f && ts <= 1.0f) {
                        throwStrength = ts;
                        state.cachedThrowStrength = ts;
                    }
                }
            }
        }

        // Get player entity to skip self-collision in traces
        auto playerEntity = static_cast<cs2::C_BaseEntity*>(playerPawn.baseEntity());

        auto simulator = hookContext.template make<GrenadeSimulator<HookContext>>();

        // Compute spawn position using hull trace (IDA verified: trace 22u forward, pull back 6u)
        // Pitch correction and Z offset are handled internally by computeSpawnPosition
        auto spawnPos = simulator.computeSpawnPosition(eyePos, viewAngles, throwStrength, static_cast<void*>(playerEntity));
        if (!spawnPos.hasValue()) {
            hideCurrentPrediction();
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

        // Read friction from config (0..200 integer → 0.000..0.200 float)
        // Use slider to find the exact friction value. 0 = no friction, 200 = 0.200.
        float frictionOverride = 0.0f;
        {
            auto frictionVar = context().config().template getVariable<grenade_prediction_vars::BounceFriction>();
            frictionOverride = static_cast<float>(static_cast<std::uint8_t>(frictionVar)) / 1000.0f;
        }

        auto& traj = state.tempTrajectory;
        simulator.simulate(traj, spawnPos.value(), initialVelocity, kind.value(), static_cast<void*>(playerEntity), frictionOverride);

        if (!traj.valid || traj.pointsCount == 0) {
            // A held grenade must never render an earlier throw after a failed simulation.
            hideCurrentPrediction();
            return;
        }

        state.cachedTrajectory = traj;
        drawTrajectory(traj);
    }

    void clearPrediction() noexcept
    {
        auto& state = context().state();
        state.cachedThrowStrength = 1.0f;
        state.cachedThrowStrengthWeapon = nullptr;
        invalidateTrajectory(state.cachedTrajectory);
        invalidateTrajectory(state.tempTrajectory);
        hideContainerPanel();
    }

private:

    void hideCurrentPrediction() noexcept
    {
        auto& state = context().state();
        invalidateTrajectory(state.tempTrajectory);
        // Do not permit a subsequent freeze-mode draw to reveal a failed throw.
        invalidateTrajectory(state.cachedTrajectory);
        hideContainerPanel();
    }

    static void invalidateTrajectory(Trajectory& trajectory) noexcept
    {
        trajectory.pointsCount = 0;
        trajectory.bouncesCount = 0;
        trajectory.valid = false;
        trajectory.validLanding = false;
    }

    void hideContainerPanel() noexcept
    {
        auto&& panel = hookContext.template make<PanelHandle>(context().state().containerPanelHandle).get();
        if (panel)
            panel.setVisible(false);
    }

    void drawTrajectory(const Trajectory& traj) noexcept
    {
        if (!traj.valid || traj.pointsCount == 0) {
            hideContainerPanel();
            return;
        }

        auto&& containerPanel = getContainerPanel();
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

    [[nodiscard]] decltype(auto) getContainerPanel() const noexcept
    {
        return hookContext.template make<PanelHandle>(context().state().containerPanelHandle)
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
