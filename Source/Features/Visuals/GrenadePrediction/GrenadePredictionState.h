#pragma once

#include <CS2/Panorama/CUIEngine.h>
#include <CS2/Panorama/PanelHandle.h>
#include <Features/Visuals/GrenadePrediction/Trajectory.h>
#include <cstdint>

struct GrenadeThrowObservation {
    [[nodiscard]] bool observePinState(const void* weapon, bool pinPulled) noexcept
    {
        if (observedWeapon != weapon) {
            observedWeapon = weapon;
            hasPinBaseline = true;
            previousPinPulled = pinPulled;
            retainedThrowStrength = 1.0f;
            hasRetainedThrowStrength = false;
            return false;
        }

        const bool released = hasPinBaseline && previousPinPulled && !pinPulled;
        const bool startedNewPull = hasPinBaseline && !previousPinPulled && pinPulled;
        if (startedNewPull) {
            retainedThrowStrength = 1.0f;
            hasRetainedThrowStrength = false;
        }
        hasPinBaseline = true;
        previousPinPulled = pinPulled;
        return released;
    }

    void retainThrowStrength(float throwStrength) noexcept
    {
        if (throwStrength >= 0.0f && throwStrength <= 1.0f)
        {
            retainedThrowStrength = throwStrength;
            hasRetainedThrowStrength = true;
        }
    }

    [[nodiscard]] bool canCommitRelease(bool releaseEdge) const noexcept
    {
        return releaseEdge && hasRetainedThrowStrength;
    }

    void reset() noexcept
    {
        observedWeapon = nullptr;
        hasPinBaseline = false;
        previousPinPulled = false;
        retainedThrowStrength = 1.0f;
        hasRetainedThrowStrength = false;
    }

    const void* observedWeapon{};
    bool hasPinBaseline{};
    bool previousPinPulled{};
    float retainedThrowStrength{1.0f};
    bool hasRetainedThrowStrength{};
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
    GrenadeThrowObservation throwObservation{};
    float lastCommitCurtime{};
    float lastValidCurtime{};
    bool hasCommitCurtime{};
    bool hasLastValidCurtime{};

    [[nodiscard]] LastGrenadeCacheVisibility cacheVisibility(bool alwaysShow, float duration, bool hasCurtime, float curtime) noexcept
    {
        if (hasCurtime) {
            const bool timeRolledBack = (hasLastValidCurtime && curtime < lastValidCurtime)
                || (hasCommitCurtime && curtime < lastCommitCurtime);
            lastValidCurtime = curtime;
            hasLastValidCurtime = true;
            if (timeRolledBack) {
                throwObservation.reset();
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
