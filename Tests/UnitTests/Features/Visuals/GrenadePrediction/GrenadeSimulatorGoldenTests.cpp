#include <gtest/gtest.h>

#include <Mocks/GrenadePrediction/ScriptedGrenadeTrace.h>

namespace
{

using Simulator = GrenadeSimulator<GrenadeSimulatorTestHookContext>;

class GrenadeSimulatorRegressionTest : public testing::Test {
protected:
    GrenadeSimulatorTestHookContext context;
    Simulator simulator{context};
    Trajectory trajectory;
};

TEST_F(GrenadeSimulatorRegressionTest, FreeFlightHeAndFlashUsePaddedTracerHorizon)
{
    // 1.5 s base + 0.125 s padding permits 105 post-step movements.
    for (const auto kind : {cs2::GrenadeKind::HEGrenade, cs2::GrenadeKind::Flashbang}) {
        context.trace.calls = context.trace.resultCount = 0;
        context.trace.clearAfterScript();
        simulator.simulate(trajectory, {}, {100.0f, 0.0f, 0.0f}, kind, nullptr);
        ASSERT_TRUE(trajectory.valid);
        EXPECT_EQ(context.trace.calls, 105);
        EXPECT_NEAR(trajectory.endPos.x, 164.0625f, 0.001f);
        EXPECT_NEAR(trajectory.endPos.z, -430.6640625f, 0.001f);
    }
}

TEST_F(GrenadeSimulatorRegressionTest, FuseAndMidairTimeoutKinematics)
{
    // 2.0 s base + 0.125 s padding permits 137 post-step movements.
    context.trace.clearAfterScript();
    simulator.simulate(trajectory, {}, {}, cs2::GrenadeKind::Molotov, nullptr);
    EXPECT_TRUE(trajectory.valid);
    EXPECT_FALSE(trajectory.validLanding);
    EXPECT_EQ(context.trace.calls, 137);
    // -0.5 * (800 * .4) * (137 / 64)² = -733.1640625.
    EXPECT_NEAR(trajectory.endPos.z, -733.1640625f, 0.001f);
}

TEST_F(GrenadeSimulatorRegressionTest, FloorBounceAppliesReflectionAndElasticity)
{
    // Reflection: -64z becomes +64.03125z, then elasticity .45 applies to all axes.
    context.trace.push(TraceResult{0.5f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}});
    context.trace.clearAfterScript();
    cs2::Vector position{};
    cs2::Vector velocity{100.0f, 0.0f, -64.0f};
    const auto result = GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade);
    ASSERT_TRUE(result.hit);
    EXPECT_NEAR(velocity.x, 45.0f, 0.001f);
    EXPECT_NEAR(velocity.z, 31.0640625f, 0.001f);
}

TEST_F(GrenadeSimulatorRegressionTest, WallBounceAppliesReflectionEquation)
{
    // After gravity, x=64 reflects with the 1/32 push-off and .45 elasticity.
    context.trace.push(TraceResult{0.5f, {}, {-1.0f, 0.0f, 0.0f}});
    context.trace.clearAfterScript();
    cs2::Vector position{};
    cs2::Vector velocity{64.0f, 0.0f, 0.0f};
    const auto result = GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade);
    ASSERT_TRUE(result.hit);
    EXPECT_NEAR(velocity.x, -28.8140625f, 0.001f);
    EXPECT_NEAR(velocity.z, -2.25f, 0.001f);
}

}
