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

TEST_F(GrenadeSimulatorRegressionTest, FreeFlightHeFuseKinematics)
{
    // Independent Verlet kinematics: 98 steps at 1/64 s, x = v*t and z = -0.5*(800*.4)*t².
    context.trace.clearAfterScript();
    simulator.simulate(trajectory, {}, {100.0f, 0.0f, 0.0f}, cs2::GrenadeKind::HEGrenade, nullptr);
    ASSERT_TRUE(trajectory.valid);
    EXPECT_NEAR(trajectory.endPos.x, 153.125f, 0.001f);
    EXPECT_NEAR(trajectory.endPos.z, -375.15625f, 0.001f);
}

TEST_F(GrenadeSimulatorRegressionTest, FuseAndMidairTimeoutKinematics)
{
    // Strict > checks occur after their step: HE/flash runs 98 steps and Molotov runs 130 steps.
    context.trace.clearAfterScript();
    simulator.simulate(trajectory, {}, {}, cs2::GrenadeKind::Flashbang, nullptr);
    EXPECT_TRUE(trajectory.valid);
    EXPECT_NEAR(trajectory.endPos.z, -375.15625f, 0.001f);
    simulator.simulate(trajectory, {}, {}, cs2::GrenadeKind::Molotov, nullptr);
    EXPECT_TRUE(trajectory.valid);
    EXPECT_FALSE(trajectory.validLanding);
    // -0.5 * (800 * .4) * (130 / 64)² = -660.15625.
    EXPECT_NEAR(trajectory.endPos.z, -660.15625f, 0.001f);
}

TEST_F(GrenadeSimulatorRegressionTest, FloorBounceAppliesReflectionAndTangentialFriction)
{
    // Reflection: -64z becomes +64.03125z then elasticity .45; x is additionally retained at .8.
    context.trace.push(TraceResult{0.5f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}});
    context.trace.clearAfterScript();
    cs2::Vector position{};
    cs2::Vector velocity{100.0f, 0.0f, -64.0f};
    const auto result = GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade, nullptr, 0.2f);
    ASSERT_TRUE(result.hit);
    EXPECT_NEAR(velocity.x, 36.0f, 0.001f);
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
