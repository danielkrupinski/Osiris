#include <gtest/gtest.h>

template <typename HookContext>
struct GrenadePredictionTestAccess;

#include <Features/Visuals/GrenadePrediction/GrenadePrediction.h>

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

}
