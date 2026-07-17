#pragma once

#include <CS2/Panorama/CUIEngine.h>
#include <CS2/Panorama/PanelHandle.h>
#include <Features/Visuals/GrenadePrediction/Trajectory.h>
#include <Features/Visuals/GrenadePrediction/GrenadeLaunchSnapshot.h>
#include <Features/Visuals/GrenadePrediction/GrenadePredictionParams.h>
#include <cstdint>

struct GrenadeThrowObservation {
    [[nodiscard]] bool observeWeapon(const void* weapon) noexcept
    {
        if (observedWeapon == weapon)
            return false;
        resetThrowSequence();
        observedWeapon = weapon;
        hasPinBaseline = false;
        previousPinPulled = false;
        return true;
    }

    [[nodiscard]] bool observePinState(const void* weapon, bool pinPulled) noexcept
    {
        if (observedWeapon != weapon) {
            static_cast<void>(observeWeapon(weapon));
            hasPinBaseline = true;
            previousPinPulled = pinPulled;
            return false;
        }

        const bool released = hasPinBaseline && previousPinPulled && !pinPulled;
        const bool startedNewPull = hasPinBaseline && !previousPinPulled && pinPulled;
        if (startedNewPull) {
            resetThrowSequence();
        }
        hasPinBaseline = true;
        previousPinPulled = pinPulled;
        return released;
    }

    void retainThrowStrength(float throwStrength) noexcept
    {
        if (!isStrengthLocked() && throwStrength >= 0.0f && throwStrength <= 1.0f)
        {
            retainedThrowStrength = throwStrength;
            hasRetainedThrowStrength = true;
        }
    }

    [[nodiscard]] bool canCommitRelease(bool releaseEdge) const noexcept
    {
        return releaseEdge && hasRetainedThrowStrength && !hasPendingThrowTime && !finalized;
    }

    [[nodiscard]] bool observeThrowTime(const void* weapon, float throwTime) noexcept
    {
        if (weapon != observedWeapon)
            return false;
        if (!(throwTime > 0.0f)) {
            if (!hasPendingThrowTime)
                return false;
            pendingThrowTime = 0.0f;
            hasPendingThrowTime = false;
            return true;
        }
        if (finalized)
            return false;
        if (!hasPendingThrowTime || pendingThrowTime != throwTime) {
            pendingThrowTime = throwTime;
            hasPendingThrowTime = true;
            return true;
        }
        return false;
    }

    [[nodiscard]] bool consumeActualExecution(bool hasCurtime, float curtime) noexcept
    {
        if (!hasCurtime || !hasPendingThrowTime || finalized || curtime <= pendingThrowTime)
            return false;
        finalized = true;
        hasPendingThrowTime = false;
        return true;
    }

    [[nodiscard]] bool consumeLegacyRelease(bool releaseEdge) noexcept
    {
        if (!releaseEdge || hasPendingThrowTime || finalized)
            return false;
        finalized = true;
        return true;
    }

    [[nodiscard]] bool hasPendingExecution() const noexcept { return hasPendingThrowTime; }
    [[nodiscard]] const void* pendingWeapon() const noexcept { return hasPendingThrowTime ? observedWeapon : nullptr; }
    [[nodiscard]] std::uint32_t pendingSequence() const noexcept { return sequence; }
    [[nodiscard]] bool canCommitActualExecution() const noexcept { return hasRetainedThrowStrength; }
    [[nodiscard]] bool isFinalized() const noexcept { return finalized; }
    [[nodiscard]] bool isStrengthLocked() const noexcept { return hasPendingThrowTime || finalized; }
    void resetThrowSequence() noexcept
    {
        retainedThrowStrength = 1.0f;
        hasRetainedThrowStrength = false;
        pendingThrowTime = 0.0f;
        hasPendingThrowTime = false;
        finalized = false;
        ++sequence;
    }

    void reset() noexcept
    {
        observedWeapon = nullptr;
        hasPinBaseline = false;
        previousPinPulled = false;
        resetThrowSequence();
    }

    const void* observedWeapon{};
    bool hasPinBaseline{};
    bool previousPinPulled{};
    float retainedThrowStrength{1.0f};
    bool hasRetainedThrowStrength{};
    float pendingThrowTime{};
    bool hasPendingThrowTime{};
    bool finalized{};
    std::uint32_t sequence{};
};

struct GrenadePredictionUpdateScheduler {
    [[nodiscard]] bool shouldUpdate(bool force, bool hasFrametime, float frametime) noexcept
    {
        if (force || !initialized) {
            initialized = true;
            accumulatedTime = 0.0f;
            return true;
        }
        if (!hasFrametime || !(frametime > 0.0f) || frametime > 0.25f) {
            accumulatedTime = 0.0f;
            return true;
        }
        if (frametime >= grenade_prediction_params::kUpdateInterval) {
            accumulatedTime = 0.0f;
            return true;
        }
        accumulatedTime += frametime;
        if (accumulatedTime < grenade_prediction_params::kUpdateInterval)
            return false;
        accumulatedTime -= grenade_prediction_params::kUpdateInterval;
        return true;
    }

    void reset() noexcept { initialized = false; accumulatedTime = 0.0f; }
    bool initialized{};
    float accumulatedTime{};
};

struct GrenadeTrajectoryPanelStyleState {
    int pointsCount{};
    int bouncesCount{};
    bool validLanding{};
    float trajectoryHue{};
    float bounceHue{};
    bool initialized{};
};

enum class LastGrenadeCacheVisibility {
    Hide,
    Show,
    Invalidate
};

struct GrenadePredictionState {
    cs2::PanelHandle liveContainerPanelHandle{};
    cs2::PanelHandle lastCacheContainerPanelHandle{};
    Trajectory lastCommittedTrajectory{};
    Trajectory tempTrajectory{};
    const void* tempTrajectoryWeapon{};
    std::uint32_t tempTrajectorySequence{};
    GrenadeThrowObservation throwObservation{};
    GrenadeLaunchSnapshot launchSnapshot{};
    float lastCommitCurtime{};
    float lastValidCurtime{};
    bool hasCommitCurtime{};
    bool hasLastValidCurtime{};
    GrenadePredictionUpdateScheduler updateScheduler{};
    int liveActivePanelCount{};
    int lastCacheActivePanelCount{};
    GrenadeTrajectoryPanelStyleState livePanelStyle{};
    GrenadeTrajectoryPanelStyleState lastCachePanelStyle{};
    bool rollbackDetected{};

    void invalidateTempTrajectory() noexcept
    {
        tempTrajectory.pointsCount = 0;
        tempTrajectory.bouncesCount = 0;
        tempTrajectory.valid = false;
        tempTrajectory.validLanding = false;
        tempTrajectoryWeapon = nullptr;
        tempTrajectorySequence = 0;
    }

    void tagTempTrajectory(const void* weapon, std::uint32_t sequence) noexcept
    {
        tempTrajectoryWeapon = weapon;
        tempTrajectorySequence = sequence;
    }

    [[nodiscard]] bool ownsTempTrajectory(const void* weapon, std::uint32_t sequence) const noexcept
    {
        return tempTrajectory.valid && tempTrajectory.pointsCount != 0
            && tempTrajectoryWeapon == weapon && tempTrajectorySequence == sequence;
    }

    [[nodiscard]] bool stageOwnedTempTrajectory(const void* weapon, std::uint32_t sequence) noexcept
    {
        if (!ownsTempTrajectory(weapon, sequence))
            return false;
        lastCommittedTrajectory = tempTrajectory;
        return true;
    }

    void finalizeStagedTrajectory(bool hasCandidate, bool hasCurtime, float curtime) noexcept
    {
        if (!hasCandidate)
            return;
        if (hasCurtime) {
            lastCommitCurtime = curtime;
            hasCommitCurtime = true;
        } else if (hasLastValidCurtime) {
            lastCommitCurtime = lastValidCurtime;
            hasCommitCurtime = true;
        }
    }

    void captureLaunchSnapshot(float stashTime, cs2::Vector shootAngles, cs2::Vector throwPosition, cs2::Vector velocity, const void* weapon, std::uint32_t sequence) noexcept
    {
        launchSnapshot.capture(stashTime, shootAngles, throwPosition, velocity, weapon, sequence);
    }

    [[nodiscard]] bool ownsLaunchSnapshot(const void* weapon, std::uint32_t sequence) const noexcept
    {
        return launchSnapshot.isOwnedBy(weapon, sequence);
    }

    void invalidateLaunchSnapshot() noexcept { launchSnapshot.invalidate(); }

    void invalidateForSequenceChange() noexcept
    {
        invalidateTempTrajectory();
        invalidateLaunchSnapshot();
    }

    void resetForRollback() noexcept
    {
        throwObservation.reset();
        invalidateTempTrajectory();
        invalidateLaunchSnapshot();
        updateScheduler.reset();
        rollbackDetected = true;
    }

    [[nodiscard]] LastGrenadeCacheVisibility cacheVisibility(bool alwaysShow, float duration, bool hasCurtime, float curtime) noexcept
    {
        if (hasCurtime) {
            const bool timeRolledBack = (hasLastValidCurtime && curtime < lastValidCurtime)
                || (hasCommitCurtime && curtime < lastCommitCurtime);
            lastValidCurtime = curtime;
            hasLastValidCurtime = true;
            if (timeRolledBack) {
                resetForRollback();
                return LastGrenadeCacheVisibility::Invalidate;
            }
        }

        if (alwaysShow)
            return lastCommittedTrajectory.valid && lastCommittedTrajectory.pointsCount != 0 ? LastGrenadeCacheVisibility::Show : LastGrenadeCacheVisibility::Hide;
        if (duration <= 0.0f)
            return LastGrenadeCacheVisibility::Invalidate;
        if (!hasCurtime || !hasCommitCurtime)
            return LastGrenadeCacheVisibility::Hide;
        return curtime - lastCommitCurtime <= duration ? LastGrenadeCacheVisibility::Show : LastGrenadeCacheVisibility::Invalidate;
    }

};
