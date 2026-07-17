#include <gtest/gtest.h>

#include <limits>

template <typename HookContext>
struct GrenadePredictionTestAccess;

#include <Features/Visuals/GrenadePrediction/GrenadePrediction.h>
#include <Features/Visuals/GrenadePrediction/GrenadePredictionConfigVariables.h>
#include <Features/Visuals/GrenadePrediction/GrenadePredictionState.h>
#include <Features/Visuals/GrenadePrediction/TrajectoryLineSegment.h>
#include <Utils/Math.h>

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

TEST(GrenadePredictionTest, ThrowTimeUsesStrictGreaterThanExecutionSemantics)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);
    static_cast<void>(observation.observePinState(weapon, true));

    EXPECT_TRUE(observation.observeThrowTime(weapon, 10.0f));
    EXPECT_FALSE(observation.consumeActualExecution(true, 10.0f));
    EXPECT_TRUE(observation.hasPendingExecution());
    EXPECT_TRUE(observation.consumeActualExecution(true, 10.01f));
    EXPECT_FALSE(observation.consumeActualExecution(true, 11.0f));
}

TEST(GrenadePredictionTest, ThrowTimeCanBeObservedWithoutPinState)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);
    EXPECT_TRUE(observation.observeWeapon(weapon));

    EXPECT_TRUE(observation.observeThrowTime(weapon, 10.0f));
    EXPECT_TRUE(observation.hasPendingExecution());
}

TEST(GrenadePredictionTest, PositiveScheduleCancelsWhenThrowTimeResetsBeforeDeadline)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);
    EXPECT_TRUE(observation.observeWeapon(weapon));
    static_cast<void>(observation.observeThrowTime(weapon, 10.0f));

    EXPECT_TRUE(observation.observeThrowTime(weapon, 0.0f));
    EXPECT_FALSE(observation.hasPendingExecution());
    EXPECT_FALSE(observation.consumeActualExecution(true, 11.0f));
}

TEST(GrenadePredictionTest, NewlyObservedOverdueThrowExecutesInSameHook)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);
    EXPECT_TRUE(observation.observeWeapon(weapon));

    EXPECT_TRUE(observation.observeThrowTime(weapon, 9.0f));
    EXPECT_TRUE(observation.consumeActualExecution(true, 10.0f));
}

TEST(GrenadePredictionTest, WeaponSwitchResetsPendingThrowAndOldFallbackRemainsEligible)
{
    GrenadeThrowObservation observation{};
    const auto oldWeapon = reinterpret_cast<const void*>(1);
    const auto newWeapon = reinterpret_cast<const void*>(2);
    EXPECT_TRUE(observation.observeWeapon(oldWeapon));
    observation.retainThrowStrength(0.5f);
    static_cast<void>(observation.observeThrowTime(oldWeapon, 5.0f));

    EXPECT_EQ(observation.pendingWeapon(), oldWeapon);
    EXPECT_TRUE(observation.canCommitActualExecution());
    EXPECT_TRUE(observation.observeWeapon(newWeapon));
    EXPECT_FALSE(observation.hasPendingExecution());
}

TEST(GrenadePredictionTest, IdleThrowTimeDoesNotScheduleExecution)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);
    static_cast<void>(observation.observePinState(weapon, true));

    EXPECT_FALSE(observation.observeThrowTime(weapon, 0.0f));
    EXPECT_FALSE(observation.observeThrowTime(weapon, -1.0f));
    EXPECT_FALSE(observation.consumeActualExecution(true, 10.0f));
}

TEST(GrenadePredictionTest, PendingThrowPreservesStrengthAndMissingCurtime)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);
    static_cast<void>(observation.observePinState(weapon, true));
    observation.retainThrowStrength(0.6f);
    static_cast<void>(observation.observeThrowTime(weapon, 5.0f));

    EXPECT_FALSE(observation.consumeActualExecution(false, 0.0f));
    EXPECT_TRUE(observation.hasPendingExecution());
    EXPECT_FLOAT_EQ(observation.retainedThrowStrength, 0.6f);
}

TEST(GrenadePredictionTest, NewPullWeaponAndResetDiscardPendingThrow)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);
    static_cast<void>(observation.observePinState(weapon, true));
    static_cast<void>(observation.observeThrowTime(weapon, 5.0f));
    static_cast<void>(observation.observePinState(weapon, false));
    static_cast<void>(observation.observePinState(weapon, true));
    EXPECT_FALSE(observation.hasPendingExecution());

    static_cast<void>(observation.observeThrowTime(weapon, 6.0f));
    observation.reset();
    EXPECT_FALSE(observation.hasPendingExecution());
    EXPECT_FALSE(observation.isFinalized());
}

TEST(GrenadePredictionTest, SchedulerHandlesInitialForceAndFrameCadence)
{
    GrenadePredictionUpdateScheduler scheduler{};
    EXPECT_TRUE(scheduler.shouldUpdate(false, true, 1.0f / 240.0f));
    EXPECT_FALSE(scheduler.shouldUpdate(false, true, 1.0f / 240.0f));
    EXPECT_TRUE(scheduler.shouldUpdate(true, true, 1.0f / 240.0f));

    GrenadePredictionUpdateScheduler sixtyHz{};
    EXPECT_TRUE(sixtyHz.shouldUpdate(false, true, 1.0f / 60.0f));
    EXPECT_TRUE(sixtyHz.shouldUpdate(false, true, 1.0f / 60.0f));
    EXPECT_TRUE(sixtyHz.shouldUpdate(false, false, 0.0f));
}

TEST(GrenadePredictionTest, SchedulerAveragesNinetyHzAtTwoFortyHz)
{
    GrenadePredictionUpdateScheduler scheduler{};
    int updates = scheduler.shouldUpdate(false, true, 1.0f / 240.0f) ? 1 : 0;
    for (int i = 0; i < 239; ++i) {
        if (scheduler.shouldUpdate(false, true, 1.0f / 240.0f))
            ++updates;
    }
    EXPECT_GE(updates, 89);
    EXPECT_LE(updates, 91);
}

TEST(GrenadePredictionTest, SchedulerFailsOpenForInvalidOrHugeFrametime)
{
    GrenadePredictionUpdateScheduler scheduler{};
    EXPECT_TRUE(scheduler.shouldUpdate(false, true, 1.0f / 240.0f));
    EXPECT_TRUE(scheduler.shouldUpdate(false, false, 0.0f));
    EXPECT_TRUE(scheduler.shouldUpdate(false, true, 0.0f));
    EXPECT_TRUE(scheduler.shouldUpdate(false, true, -0.01f));
    EXPECT_TRUE(scheduler.shouldUpdate(false, true, std::numeric_limits<float>::quiet_NaN()));
    EXPECT_TRUE(scheduler.shouldUpdate(false, true, 1.0f / 60.0f));
    EXPECT_TRUE(scheduler.shouldUpdate(false, true, 0.3f));
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

TEST(GrenadePredictionTest, TempOwnershipRejectsDifferentWeaponAndSequence)
{
    GrenadePredictionState state{};
    const auto weapon = reinterpret_cast<const void*>(1);
    state.tempTrajectory.valid = true;
    state.tempTrajectory.pointsCount = 1;
    state.tagTempTrajectory(weapon, 7);

    EXPECT_TRUE(state.ownsTempTrajectory(weapon, 7));
    EXPECT_FALSE(state.ownsTempTrajectory(reinterpret_cast<const void*>(2), 7));
    EXPECT_FALSE(state.ownsTempTrajectory(weapon, 8));
}

TEST(GrenadePredictionTest, FirstOverdueNewWeaponCannotStageOldSnapshot)
{
    GrenadePredictionState state{};
    const auto oldWeapon = reinterpret_cast<const void*>(1);
    const auto newWeapon = reinterpret_cast<const void*>(2);
    state.tempTrajectory.valid = true;
    state.tempTrajectory.pointsCount = 1;
    state.tagTempTrajectory(oldWeapon, 3);

    EXPECT_FALSE(state.stageOwnedTempTrajectory(newWeapon, 4));
}

TEST(GrenadePredictionTest, FinalizedStatePersistsUntilNewPull)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);
    EXPECT_TRUE(observation.observeWeapon(weapon));
    observation.retainThrowStrength(0.5f);
    static_cast<void>(observation.observeThrowTime(weapon, 2.0f));
    EXPECT_TRUE(observation.consumeActualExecution(true, 3.0f));
    EXPECT_TRUE(observation.isFinalized());
    EXPECT_FALSE(observation.observeThrowTime(weapon, 4.0f));
    static_cast<void>(observation.observePinState(weapon, false));
    static_cast<void>(observation.observePinState(weapon, true));
    EXPECT_FALSE(observation.isFinalized());
}

TEST(GrenadePredictionTest, RetainedStrengthLocksPendingAndFinalizedButUnlocksOnNewPull)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);
    static_cast<void>(observation.observePinState(weapon, true));
    observation.retainThrowStrength(0.25f);
    static_cast<void>(observation.observeThrowTime(weapon, 2.0f));
    observation.retainThrowStrength(0.75f);
    EXPECT_FLOAT_EQ(observation.retainedThrowStrength, 0.25f);
    static_cast<void>(observation.consumeActualExecution(true, 3.0f));
    observation.retainThrowStrength(0.5f);
    EXPECT_FLOAT_EQ(observation.retainedThrowStrength, 0.25f);
    static_cast<void>(observation.observePinState(weapon, false));
    static_cast<void>(observation.observePinState(weapon, true));
    observation.retainThrowStrength(0.5f);
    EXPECT_FLOAT_EQ(observation.retainedThrowStrength, 0.5f);
}

TEST(GrenadePredictionTest, LegacyReleaseFinalizesOnceWithoutStrength)
{
    GrenadeThrowObservation observation{};
    const auto weapon = reinterpret_cast<const void*>(1);
    static_cast<void>(observation.observePinState(weapon, true));
    const bool release = observation.observePinState(weapon, false);
    EXPECT_TRUE(observation.consumeLegacyRelease(release));
    EXPECT_TRUE(observation.isFinalized());
    EXPECT_FALSE(observation.consumeLegacyRelease(release));
}

TEST(GrenadePredictionTest, SchedulerClearsDebtAfterSlowOrInvalidFrame)
{
    GrenadePredictionUpdateScheduler scheduler{};
    EXPECT_TRUE(scheduler.shouldUpdate(false, true, 1.0f / 240.0f));
    EXPECT_FALSE(scheduler.shouldUpdate(false, true, 1.0f / 240.0f));
    EXPECT_GT(scheduler.accumulatedTime, 0.0f);
    EXPECT_TRUE(scheduler.shouldUpdate(false, true, 1.0f / 60.0f));
    EXPECT_FLOAT_EQ(scheduler.accumulatedTime, 0.0f);
    EXPECT_FALSE(scheduler.shouldUpdate(false, true, 1.0f / 240.0f));
    EXPECT_TRUE(scheduler.shouldUpdate(false, false, 0.0f));
    EXPECT_FLOAT_EQ(scheduler.accumulatedTime, 0.0f);
}

TEST(GrenadePredictionTest, RollbackInvalidatesOwnedTempAndResetsScheduler)
{
    GrenadePredictionState state{};
    state.tempTrajectory.valid = true;
    state.tempTrajectory.pointsCount = 1;
    state.tagTempTrajectory(reinterpret_cast<const void*>(1), 1);
    static_cast<void>(state.updateScheduler.shouldUpdate(false, true, 1.0f / 240.0f));
    state.resetForRollback();

    EXPECT_FALSE(state.tempTrajectory.valid);
    EXPECT_FALSE(state.ownsTempTrajectory(reinterpret_cast<const void*>(1), 1));
    EXPECT_FALSE(state.updateScheduler.initialized);
    EXPECT_TRUE(state.rollbackDetected);
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

TEST(GrenadePredictionTest, LaunchSnapshotFreshnessMatchesEngineWindow)
{
    EXPECT_FALSE(GrenadeLaunchSnapshot::isFresh(false, true, 10.1f, 10.0f));
    EXPECT_FALSE(GrenadeLaunchSnapshot::isFresh(true, false, 10.1f, 10.0f));
    EXPECT_FALSE(GrenadeLaunchSnapshot::isFresh(true, true, 10.0f, 10.0f));
    EXPECT_FALSE(GrenadeLaunchSnapshot::isFresh(true, true, 9.9f, 10.0f));
    EXPECT_FALSE(GrenadeLaunchSnapshot::isFresh(true, true, 10.21f, 10.0f));
    EXPECT_TRUE(GrenadeLaunchSnapshot::isFresh(true, true, 10.01f, 10.0f));
    EXPECT_TRUE(GrenadeLaunchSnapshot::isFresh(true, true, 10.2f, 10.0f));
}

TEST(GrenadePredictionTest, LaunchSnapshotOwnershipRejectsDifferentWeaponAndSequence)
{
    GrenadePredictionState state{};
    const auto weapon = reinterpret_cast<const void*>(1);
    state.captureLaunchSnapshot(1.0f, {}, {}, {}, weapon, 7);

    EXPECT_TRUE(state.ownsLaunchSnapshot(weapon, 7));
    EXPECT_FALSE(state.ownsLaunchSnapshot(reinterpret_cast<const void*>(2), 7));
    EXPECT_FALSE(state.ownsLaunchSnapshot(weapon, 8));
}

TEST(GrenadePredictionTest, SequenceChangeAndRollbackInvalidateLaunchSnapshot)
{
    GrenadePredictionState state{};
    const auto weapon = reinterpret_cast<const void*>(1);
    state.captureLaunchSnapshot(1.0f, {}, {}, {}, weapon, 7);
    state.invalidateForSequenceChange();
    EXPECT_FALSE(state.ownsLaunchSnapshot(weapon, 7));

    state.captureLaunchSnapshot(1.0f, {}, {}, {}, weapon, 8);
    state.resetForRollback();
    EXPECT_FALSE(state.ownsLaunchSnapshot(weapon, 8));
}

TEST(GrenadePredictionTest, LaunchSnapshotCaptureOwnershipFollowsActiveLifecycle)
{
    const auto oldWeapon = reinterpret_cast<const void*>(1);
    const auto activeWeapon = reinterpret_cast<const void*>(2);

    EXPECT_TRUE(GrenadeLaunchSnapshot::shouldCaptureForLifecycle(true, activeWeapon, 7, activeWeapon, 7, false, false));
    EXPECT_FALSE(GrenadeLaunchSnapshot::shouldCaptureForLifecycle(true, oldWeapon, 6, activeWeapon, 7, true, true));
    EXPECT_TRUE(GrenadeLaunchSnapshot::shouldCaptureForLifecycle(false, nullptr, 0, activeWeapon, 7, true, false));
    EXPECT_FALSE(GrenadeLaunchSnapshot::shouldCaptureForLifecycle(false, nullptr, 0, activeWeapon, 7, false, false));
    EXPECT_TRUE(GrenadeLaunchSnapshot::shouldCaptureForLifecycle(false, nullptr, 0, activeWeapon, 7, false, true));
}

TEST(GrenadePredictionTest, Atan2MatchesReferenceAngles)
{
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kHalfPi = kPi * 0.5f;
    struct TestCase { float y; float x; float expected; };
    constexpr TestCase cases[]{
        {.25f, 1.0f, 0.2449786631f}, {.5f, 1.0f, 0.4636476090f}, {1.0f, 1.0f, kPi * 0.25f},
        {2.0f, 1.0f, 1.1071487178f}, {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, kPi},
        {1.0f, 0.0f, kHalfPi}, {-1.0f, 0.0f, -kHalfPi},
        {1.0f, -1.0f, kPi * 0.75f}, {-1.0f, -1.0f, -kPi * 0.75f}, {-1.0f, 1.0f, -kPi * 0.25f}};
    for (const auto& test : cases)
        EXPECT_NEAR(Math::atan2(test.y, test.x), test.expected, 0.001f);
}

TEST(GrenadePredictionTest, TrajectoryLineSegmentBuildsHorizontalGeometry)
{
    TrajectoryLineSegment segment{};
    EXPECT_TRUE(TrajectoryLineSegment::fromClipSpace({-.5f, 0.0f, 0.0f, 1.0f}, {.5f, 0.0f, 0.0f, 1.0f}, 1.0f, segment));
    EXPECT_FLOAT_EQ(segment.midpointX, 50.0f);
    EXPECT_FLOAT_EQ(segment.midpointY, 50.0f);
    EXPECT_FLOAT_EQ(segment.width, 50.0f);
    EXPECT_FLOAT_EQ(segment.angleDegrees, 0.0f);
}

TEST(GrenadePredictionTest, TrajectoryLineSegmentBuildsAspectCorrectVerticalGeometry)
{
    TrajectoryLineSegment segment{};
    EXPECT_TRUE(TrajectoryLineSegment::fromClipSpace({0.0f, .5f, 0.0f, 1.0f}, {0.0f, -.5f, 0.0f, 1.0f}, 2.0f, segment));
    EXPECT_FLOAT_EQ(segment.midpointX, 50.0f);
    EXPECT_FLOAT_EQ(segment.midpointY, 50.0f);
    EXPECT_FLOAT_EQ(segment.width, 25.0f);
    EXPECT_FLOAT_EQ(segment.angleDegrees, 90.0f);
}

TEST(GrenadePredictionTest, TrajectoryLineSegmentBuildsAccurateNonAxisAngle)
{
    TrajectoryLineSegment segment{};
    EXPECT_TRUE(TrajectoryLineSegment::fromClipSpace({0.0f, 0.0f, 0.0f, 1.0f}, {.5f, -.25f, 0.0f, 1.0f}, 1.0f, segment));
    EXPECT_NEAR(segment.angleDegrees, 26.565051f, 0.01f);
}

TEST(GrenadePredictionTest, TrajectoryLineSegmentClipsToViewport)
{
    TrajectoryLineSegment segment{};
    EXPECT_TRUE(TrajectoryLineSegment::fromClipSpace({-2.0f, 0.0f, 0.0f, 1.0f}, {2.0f, 0.0f, 0.0f, 1.0f}, 1.0f, segment));
    EXPECT_FLOAT_EQ(segment.midpointX, 50.0f);
    EXPECT_FLOAT_EQ(segment.width, 100.0f);
}

TEST(GrenadePredictionTest, TrajectoryLineSegmentClipsNearPlane)
{
    TrajectoryLineSegment segment{};
    EXPECT_TRUE(TrajectoryLineSegment::fromClipSpace({-.0005f, 0.0f, 0.0f, 0.0f}, {.5f, 0.0f, 0.0f, 1.0f}, 1.0f, segment));
    EXPECT_GT(segment.width, 0.0f);
}

TEST(GrenadePredictionTest, TrajectoryLineSegmentRejectsBothBehindNearPlane)
{
    TrajectoryLineSegment segment{};
    EXPECT_FALSE(TrajectoryLineSegment::fromClipSpace({0.0f, 0.0f, 0.0f, 0.0f}, {.5f, 0.0f, 0.0f, -.1f}, 1.0f, segment));
}

TEST(GrenadePredictionTest, TrajectoryLineSegmentRejectsNonFiniteInputs)
{
    TrajectoryLineSegment segment{};
    const float nan = std::numeric_limits<float>::quiet_NaN();
    EXPECT_FALSE(TrajectoryLineSegment::fromClipSpace({nan, 0.0f, 0.0f, 1.0f}, {.5f, 0.0f, 0.0f, 1.0f}, 1.0f, segment));
}

}
