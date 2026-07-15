#include <gtest/gtest.h>

#include <Features/Visuals/GrenadePrediction/GrenadePredictionParams.h>
#include <Mocks/GrenadePrediction/ScriptedGrenadeTrace.h>

namespace
{

using Simulator = GrenadeSimulator<GrenadeSimulatorTestHookContext>;

TEST(GrenadeSimulatorPureTest, ConvertsCardinalAnglesToForwardVectors)
{
    EXPECT_NEAR(Simulator::forwardFromAngles(0.0f, 0.0f).x, 1.0f, 0.0001f);
    EXPECT_NEAR(Simulator::forwardFromAngles(0.0f, 90.0f).y, 1.0f, 0.0001f);
    EXPECT_NEAR(Simulator::forwardFromAngles(-90.0f, 0.0f).z, 1.0f, 0.0001f);
    EXPECT_NEAR(Simulator::forwardFromAngles(0.0f, 180.0f).x, -1.0f, 0.0001f);
    EXPECT_NEAR(Simulator::forwardFromAngles(0.0f, -180.0f).x, -1.0f, 0.0001f);
}

TEST(GrenadeSimulatorPureTest, CorrectsPitchAndAppliesThrowDeadzoneAndVelocityClamp)
{
    const auto corrected = Simulator::computeInitialVelocity({45.0f, 0.0f, 0.0f}, 1000.0f, 0.5f);
    constexpr float correctedSpeed = 0.65f * 750.0f;
    float sine;
    float cosine;
    Math::sincos(40.0f * 3.14159265f / 180.0f, sine, cosine);
    EXPECT_NEAR(corrected.x, correctedSpeed * cosine, 0.001f);
    EXPECT_NEAR(corrected.z, -correctedSpeed * sine, 0.001f);
    EXPECT_NEAR(corrected.squareLength(), correctedSpeed * correctedSpeed, 0.1f);

    const auto atLowerBoundary = Simulator::computeInitialVelocity({}, 10.0f, 0.4f);
    const auto justInsideLowerBoundary = Simulator::computeInitialVelocity({}, 10.0f, 0.4001f);
    const auto justInsideUpperBoundary = Simulator::computeInitialVelocity({}, 10.0f, 0.5999f);
    const auto atUpperBoundary = Simulator::computeInitialVelocity({}, 10.0f, 0.6f);
    const auto commonDirection = Simulator::forwardFromAngles(-10.0f, 0.0f);

    EXPECT_NEAR(atLowerBoundary.squareLength(), 8.7f * 8.7f, 0.001f);
    EXPECT_NEAR(justInsideLowerBoundary.squareLength(), 9.75f * 9.75f, 0.001f);
    EXPECT_NEAR(justInsideUpperBoundary.squareLength(), 9.75f * 9.75f, 0.001f);
    EXPECT_NEAR(atUpperBoundary.squareLength(), 10.8f * 10.8f, 0.001f);
    EXPECT_NEAR(justInsideLowerBoundary.x, justInsideUpperBoundary.x, 0.001f);
    EXPECT_NEAR(justInsideLowerBoundary.z, justInsideUpperBoundary.z, 0.001f);
    EXPECT_NEAR(atLowerBoundary.x / 8.7f, commonDirection.x, 0.001f);
    EXPECT_NEAR(atLowerBoundary.z / 8.7f, commonDirection.z, 0.001f);
    EXPECT_NE(atLowerBoundary.squareLength(), justInsideLowerBoundary.squareLength());
    EXPECT_NE(atUpperBoundary.squareLength(), justInsideUpperBoundary.squareLength());
}

TEST(GrenadeSimulatorPureTest, ForwardVectorsRemainUnitLengthAcrossAngles)
{
    for (int pitch = -90; pitch <= 90; pitch += 15) {
        for (int yaw = -180; yaw <= 180; yaw += 15) {
            const auto forward = Simulator::forwardFromAngles(static_cast<float>(pitch), static_cast<float>(yaw));
            EXPECT_NEAR(forward.squareLength(), 1.0f, 0.00001f);
        }
    }
}

TEST(GrenadeSimulatorPureTest, ClipsOnlyVelocityMovingIntoSurface)
{
    EXPECT_NEAR(Simulator::clipVelocity({0.0f, 0.0f, -10.0f}, {0.0f, 0.0f, 1.0f}, 2.0f).z, 10.03125f, 0.0001f);
    EXPECT_EQ(Simulator::clipVelocity({0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, 1.0f}, 2.0f).z, 10.0f);
    EXPECT_EQ(Simulator::clipVelocity({10.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, 2.0f).x, 10.0f);
}

TEST(GrenadeSimulatorPureTest, FrictionDampsOnlyTheTangentialBounceComponent)
{
    GrenadeSimulatorTestHookContext context;
    Simulator simulator{context};
    context.trace.push(TraceResult{0.5f, {}, {0.0f, 0.0f, 1.0f}});
    context.trace.clearAfterScript();
    cs2::Vector position{};
    cs2::Vector velocity{100.0f, 0.0f, -64.0f};
    GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade, nullptr, 0.2f);
    EXPECT_NEAR(velocity.x, 36.0f, 0.001f);
    EXPECT_NEAR(velocity.z, 31.0640625f, 0.001f);
}

TEST(GrenadeSimulatorPureTest, DetonatesStrictlyAfterFuseBoundary)
{
    GrenadeSimulatorTestHookContext context;
    Simulator simulator{context};
    EXPECT_FALSE(GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::shouldDetonate(simulator, cs2::GrenadeKind::HEGrenade, 96));
    EXPECT_TRUE(GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::shouldDetonate(simulator, cs2::GrenadeKind::HEGrenade, 97));
    EXPECT_FALSE(GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::shouldDetonate(simulator, cs2::GrenadeKind::Molotov, 128));
    EXPECT_TRUE(GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::shouldDetonate(simulator, cs2::GrenadeKind::Molotov, 129));
}

}
