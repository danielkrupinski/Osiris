#include <gtest/gtest.h>

template <typename HookContext>
struct GrenadePredictionTestAccess;

#include <Features/Visuals/GrenadePrediction/GrenadePrediction.h>
#include <Features/Visuals/GrenadePrediction/GrenadePredictionConfigVariables.h>
#include <Features/Visuals/GrenadePrediction/GrenadePredictionState.h>

struct GrenadePredictionTestHookContext;

template <typename HookContext>
struct GrenadePredictionTestAccess {
    static void invalidateTrajectory(Trajectory& trajectory) noexcept
    {
        GrenadePrediction<HookContext>::invalidateTrajectory(trajectory);
    }
};

namespace
{

TEST(GrenadePredictionTest, InvalidatingFailedPredictionCannotLeaveRenderableTrajectory)
{
    Trajectory trajectory{};
    trajectory.pointsCount = 4;
    trajectory.bouncesCount = 2;
    trajectory.valid = true;
    trajectory.validLanding = true;

    GrenadePredictionTestAccess<GrenadePredictionTestHookContext>::invalidateTrajectory(trajectory);

    EXPECT_EQ(trajectory.pointsCount, 0);
    EXPECT_EQ(trajectory.bouncesCount, 0);
    EXPECT_FALSE(trajectory.valid);
    EXPECT_FALSE(trajectory.validLanding);
}

TEST(GrenadePredictionTest, EmptyTrajectoryRemainsHiddenAfterRepeatedInvalidation)
{
    Trajectory trajectory{};

    GrenadePredictionTestAccess<GrenadePredictionTestHookContext>::invalidateTrajectory(trajectory);
    GrenadePredictionTestAccess<GrenadePredictionTestHookContext>::invalidateTrajectory(trajectory);

    EXPECT_EQ(trajectory.pointsCount, 0);
    EXPECT_EQ(trajectory.bouncesCount, 0);
    EXPECT_FALSE(trajectory.valid);
    EXPECT_FALSE(trajectory.validLanding);
}

TEST(GrenadePredictionTest, InitialFalsePinStateOnlyEstablishesBaseline)
{
    GrenadeThrowObservation observation{};

    EXPECT_FALSE(observation.observePinState(reinterpret_cast<const void*>(1), false));
    EXPECT_FALSE(observation.observePinState(reinterpret_cast<const void*>(1), false));
}

TEST(GrenadePredictionTest, SameWeaponPinReleaseIsDetectedExactlyOnce)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);

    EXPECT_FALSE(observation.observePinState(weapon, true));
    EXPECT_TRUE(observation.observePinState(weapon, false));
    EXPECT_FALSE(observation.observePinState(weapon, false));
}

TEST(GrenadePredictionTest, LatestValidPinnedStrengthIsRetainedForRelease)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);
    static_cast<void>(observation.observePinState(weapon, true));
    observation.retainThrowStrength(0.25f);
    observation.retainThrowStrength(0.75f);
    observation.retainThrowStrength(2.0f);

    EXPECT_TRUE(observation.observePinState(weapon, false));
    EXPECT_FLOAT_EQ(observation.retainedThrowStrength, 0.75f);
    EXPECT_TRUE(observation.hasRetainedThrowStrength);
}

TEST(GrenadePredictionTest, ReleaseWithoutValidStrengthIsNotCommitEligible)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);

    EXPECT_FALSE(observation.observePinState(weapon, true));
    const bool releaseEdge = observation.observePinState(weapon, false);

    EXPECT_TRUE(releaseEdge);
    EXPECT_FALSE(observation.canCommitRelease(releaseEdge));
}

TEST(GrenadePredictionTest, InvalidStrengthDoesNotEstablishRetainedStrength)
{
    GrenadeThrowObservation observation{};

    observation.retainThrowStrength(-0.1f);
    observation.retainThrowStrength(1.1f);

    EXPECT_FALSE(observation.hasRetainedThrowStrength);
    EXPECT_FLOAT_EQ(observation.retainedThrowStrength, 1.0f);
}

TEST(GrenadePredictionTest, FirstObservedPinnedWeaponRetainsItsStrength)
{
    GrenadeThrowObservation observation{};

    EXPECT_FALSE(observation.observePinState(reinterpret_cast<const void*>(1), true));
    observation.retainThrowStrength(0.4f);

    EXPECT_FLOAT_EQ(observation.retainedThrowStrength, 0.4f);
}

TEST(GrenadePredictionTest, NewPullCycleResetsStaleStrengthBeforeRead)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);
    static_cast<void>(observation.observePinState(weapon, true));
    observation.retainThrowStrength(0.3f);
    static_cast<void>(observation.observePinState(weapon, false));

    EXPECT_FALSE(observation.observePinState(weapon, true));
    EXPECT_FLOAT_EQ(observation.retainedThrowStrength, 1.0f);
    EXPECT_FALSE(observation.hasRetainedThrowStrength);
}

TEST(GrenadePredictionTest, ResetClearsRetainedStrengthValidity)
{
    GrenadeThrowObservation observation{};

    observation.retainThrowStrength(0.4f);
    observation.reset();

    EXPECT_FALSE(observation.hasRetainedThrowStrength);
    EXPECT_FLOAT_EQ(observation.retainedThrowStrength, 1.0f);
}

TEST(GrenadePredictionTest, WeaponSwitchCannotCreateReleaseEdge)
{
    GrenadeThrowObservation observation{};

    EXPECT_FALSE(observation.observePinState(reinterpret_cast<const void*>(1), true));
    EXPECT_FALSE(observation.observePinState(reinterpret_cast<const void*>(2), false));
}

TEST(GrenadePredictionTest, AlwaysShowOverridesZeroDuration)
{
    GrenadePredictionState state{};
    state.lastCommittedTrajectory.valid = true;
    state.lastCommittedTrajectory.pointsCount = 1;

    EXPECT_EQ(state.cacheVisibility(false, 0.0f, true, 10.0f), LastGrenadeCacheVisibility::Invalidate);
    EXPECT_EQ(state.cacheVisibility(true, 0.0f, false, 0.0f), LastGrenadeCacheVisibility::Show);
}

TEST(GrenadePredictionTest, CacheDurationIncludesExactBoundaryAndExpiresAfterIt)
{
    GrenadePredictionState state{};
    state.lastCommittedTrajectory.valid = true;
    state.lastCommittedTrajectory.pointsCount = 1;
    state.hasCommitCurtime = true;
    state.lastCommitCurtime = 10.0f;

    EXPECT_EQ(state.cacheVisibility(false, 5.0f, true, 15.0f), LastGrenadeCacheVisibility::Show);
    EXPECT_EQ(state.cacheVisibility(false, 5.0f, true, 15.1f), LastGrenadeCacheVisibility::Invalidate);
    GrenadePredictionState shortDurationState{};
    shortDurationState.lastCommittedTrajectory.valid = true;
    shortDurationState.lastCommittedTrajectory.pointsCount = 1;
    shortDurationState.hasCommitCurtime = true;
    shortDurationState.lastCommitCurtime = 0.0f;
    EXPECT_EQ(shortDurationState.cacheVisibility(false, 0.1f, true, 0.1f), LastGrenadeCacheVisibility::Show);
    EXPECT_EQ(shortDurationState.cacheVisibility(false, 0.1f, true, 0.11f), LastGrenadeCacheVisibility::Invalidate);
}

TEST(GrenadePredictionTest, CacheDurationNormalizationRoundsArbitraryPrecisionToTenths)
{
    EXPECT_FLOAT_EQ(grenade_prediction_vars::normalizeCacheDuration(1.25f), 1.3f);
    EXPECT_FLOAT_EQ(grenade_prediction_vars::normalizeCacheDuration(1.23f), 1.2f);
    EXPECT_FLOAT_EQ(grenade_prediction_vars::normalizeCacheDuration(0.01f), 0.1f);
    EXPECT_FLOAT_EQ(grenade_prediction_vars::normalizeCacheDuration(60.01f), 60.0f);
}

TEST(GrenadePredictionTest, TimeRollbackInvalidatesCacheAndObservation)
{
    GrenadePredictionState state{};
    state.lastCommittedTrajectory.valid = true;
    state.lastCommittedTrajectory.pointsCount = 1;
    state.hasCommitCurtime = true;
    state.lastCommitCurtime = 10.0f;
    static_cast<void>(state.throwObservation.observePinState(reinterpret_cast<const void*>(1), true));
    static_cast<void>(state.cacheVisibility(false, 5.0f, true, 12.0f));

    EXPECT_EQ(state.cacheVisibility(false, 5.0f, true, 11.0f), LastGrenadeCacheVisibility::Invalidate);
    EXPECT_EQ(state.throwObservation.observedWeapon, nullptr);
}

TEST(GrenadePredictionTest, InvalidatingLiveTrajectoryDoesNotInvalidateLastCache)
{
    GrenadePredictionState state{};
    state.tempTrajectory.valid = true;
    state.tempTrajectory.pointsCount = 1;
    state.lastCommittedTrajectory.valid = true;
    state.lastCommittedTrajectory.pointsCount = 1;

    GrenadePredictionTestAccess<GrenadePredictionTestHookContext>::invalidateTrajectory(state.tempTrajectory);

    EXPECT_FALSE(state.tempTrajectory.valid);
    EXPECT_TRUE(state.lastCommittedTrajectory.valid);
    EXPECT_EQ(state.lastCommittedTrajectory.pointsCount, 1);
}

}
