#include <gtest/gtest.h>

#include <Mocks/GrenadePrediction/ScriptedGrenadeTrace.h>

namespace
{

using Simulator = GrenadeSimulator<GrenadeSimulatorTestHookContext>;

class GrenadeSimulatorTraceTest : public testing::Test {
protected:
    GrenadeSimulatorTestHookContext context;
    Simulator simulator{context};
    Trajectory trajectory; // Keep the >4 KB production object out of individual test stack frames.
};

TEST_F(GrenadeSimulatorTraceTest, SpawnHandlesNoHitHitPullbackAndTraceFailure)
{
    context.trace.push(TraceResult{1.0f, {}, {}});
    ASSERT_TRUE(simulator.computeSpawnPosition({}, {}, 1.0f, nullptr).hasValue());
    context.trace.push(TraceResult{0.1f, {2.0f, 0.0f, 0.0f}, {}});
    EXPECT_NEAR(simulator.computeSpawnPosition({}, {}, 1.0f, nullptr).value().x, 0.0f, 0.001f);
    context.trace.push({});
    EXPECT_FALSE(simulator.computeSpawnPosition({}, {}, 1.0f, nullptr).hasValue());
}

TEST_F(GrenadeSimulatorTraceTest, SpawnUsesTheSameStrictThrowStrengthNormalization)
{
    const auto forward = Simulator::forwardFromAngles(-10.0f, 0.0f);
    const auto expectedZ = [forward](float strength) {
        return strength * 12.0f - 12.0f + forward.z * (22.0f - 6.0f);
    };
    context.trace.push(TraceResult{1.0f, {}, {}});
    EXPECT_NEAR(simulator.computeSpawnPosition({}, {}, 0.4f, nullptr).value().z, expectedZ(0.4f), 0.001f);
    context.trace.push(TraceResult{1.0f, {}, {}});
    EXPECT_NEAR(simulator.computeSpawnPosition({}, {}, 0.4001f, nullptr).value().z, expectedZ(0.5f), 0.001f);
    context.trace.push(TraceResult{1.0f, {}, {}});
    EXPECT_NEAR(simulator.computeSpawnPosition({}, {}, 0.6f, nullptr).value().z, expectedZ(0.6f), 0.001f);
}

TEST_F(GrenadeSimulatorTraceTest, DistinguishesFreeCollisionAndTraceFailure)
{
    context.trace.push(TraceResult{1.0f, {}, {}});
    cs2::Vector pos{};
    cs2::Vector velocity{64.0f, 0.0f, 0.0f};
    EXPECT_FALSE(GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, pos, velocity, cs2::GrenadeKind::HEGrenade).hit);

    context.trace.push(TraceResult{0.5f, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}});
    context.trace.clearAfterScript();
    auto collision = GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, pos, velocity, cs2::GrenadeKind::HEGrenade);
    EXPECT_TRUE(collision.traceSucceeded);
    EXPECT_TRUE(collision.hit);

    context.trace.calls = context.trace.resultCount = 0;
    context.trace.fallback = {};
    context.trace.push({});
    EXPECT_FALSE(GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, pos, velocity, cs2::GrenadeKind::HEGrenade).traceSucceeded);
}

TEST_F(GrenadeSimulatorTraceTest, RecognizesFloorWallAndSlopeContactsByFraction)
{
    const TraceResult contacts[] = {
        {0.5f, {}, {0.0f, 0.0f, 1.0f}},
        {0.5f, {}, {-1.0f, 0.0f, 0.0f}},
        {0.5f, {}, {0.0f, 0.5f, 0.8660254f}}
    };
    for (const auto& contact : contacts) {
        context.trace.push(contact);
        context.trace.clearAfterScript();
        cs2::Vector position{};
        cs2::Vector velocity{100.0f, 0.0f, -64.0f};
        EXPECT_TRUE(GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade).hit);
        context.trace.calls = context.trace.resultCount = 0;
    }
}

TEST_F(GrenadeSimulatorTraceTest, AbortsSimulationForPrimaryOrPostBounceTraceFailure)
{
    context.trace.push({});
    simulator.simulate(trajectory, {}, {64.0f, 0.0f, 0.0f}, cs2::GrenadeKind::HEGrenade, nullptr);
    EXPECT_FALSE(trajectory.valid);
    EXPECT_EQ(trajectory.pointsCount, 0);

    context.trace.calls = context.trace.resultCount = 0;
    context.trace.push(TraceResult{0.5f, {0.5f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}});
    context.trace.push({});
    simulator.simulate(trajectory, {}, {64.0f, 0.0f, 0.0f}, cs2::GrenadeKind::HEGrenade, nullptr);
    EXPECT_FALSE(trajectory.valid);
    EXPECT_EQ(trajectory.pointsCount, 0);
}

TEST_F(GrenadeSimulatorTraceTest, CapsCornerResolutionAtFourPostBouncePasses)
{
    context.trace.push(TraceResult{0.5f, {}, {-1.0f, 0.0f, 0.0f}});
    for (int i = 0; i < 4; ++i)
        context.trace.push(TraceResult{0.5f, {}, {0.0f, -1.0f, 0.0f}});
    cs2::Vector position{};
    cs2::Vector velocity{1000.0f, 0.0f, 0.0f};
    EXPECT_TRUE(GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade).traceSucceeded);
    EXPECT_EQ(context.trace.calls, 5);
}

TEST_F(GrenadeSimulatorTraceTest, SecondaryContactsApplyResponseAndCanDetonateMolotov)
{
    context.trace.push(TraceResult{0.5f, {}, {-1.0f, 0.0f, 0.0f}});
    context.trace.push(TraceResult{0.5f, {}, {0.0f, 0.0f, 1.0f}});
    context.trace.clearAfterScript();
    cs2::Vector position{};
    cs2::Vector velocity{1000.0f, 0.0f, -64.0f};
    const auto result = GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade);
    EXPECT_EQ(result.contactsCount, 2);
    EXPECT_GT(velocity.squareLength(), 0.0f);

    context.trace.calls = context.trace.resultCount = 0;
    context.trace.push(TraceResult{0.5f, {}, {-1.0f, 0.0f, 0.0f}});
    context.trace.push(TraceResult{0.5f, {}, {0.0f, 0.0f, 1.0f}});
    cs2::Vector molotovVelocity{1000.0f, 0.0f, -64.0f};
    const auto molotov = GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, molotovVelocity, cs2::GrenadeKind::Molotov);
    EXPECT_EQ(molotov.contactsCount, 2);
    EXPECT_TRUE(molotov.impactDetonate);
    EXPECT_EQ(molotovVelocity.squareLength(), 0.0f);
}

TEST_F(GrenadeSimulatorTraceTest, CapsRecordedPointsAndBounces)
{
    context.trace.clearAfterScript();
    simulator.simulate(trajectory, {}, {100.0f, 0.0f, 0.0f}, cs2::GrenadeKind::SmokeGrenade, nullptr);
    EXPECT_LE(trajectory.pointsCount, 500);
    EXPECT_LE(trajectory.bouncesCount, 20);
}

TEST_F(GrenadeSimulatorTraceTest, BounceLimitCountsEveryContactAndCapsMarkers)
{
    for (int i = 0; i <= 20; ++i) {
        context.trace.push(TraceResult{0.5f, {}, {-1.0f, 0.0f, 0.0f}});
        context.trace.push(TraceResult{1.0f, {}, {}});
    }
    simulator.simulate(trajectory, {}, {1000.0f, 0.0f, 0.0f}, cs2::GrenadeKind::SmokeGrenade, nullptr);
    EXPECT_TRUE(trajectory.valid);
    EXPECT_EQ(trajectory.bouncesCount, 20);
}

TEST_F(GrenadeSimulatorTraceTest, SmokeFreeFlightReachesSafetyCapWithoutExceedingPointStorage)
{
    context.trace.clearAfterScript();
    simulator.simulate(trajectory, {}, {100.0f, 0.0f, 0.0f}, cs2::GrenadeKind::SmokeGrenade, nullptr);
    EXPECT_TRUE(trajectory.valid);
    EXPECT_LE(trajectory.pointsCount, 500);
    EXPECT_NEAR(trajectory.endPos.x, 1803.125f, 0.001f); // 1154 * 100 / 64
}

TEST_F(GrenadeSimulatorTraceTest, SmokeAndDecoyDeployWhenCollisionStopsDisplacement)
{
    for (const auto kind : {cs2::GrenadeKind::SmokeGrenade, cs2::GrenadeKind::Decoy}) {
        context.trace.calls = context.trace.resultCount = 0;
        context.trace.push(TraceResult{0.5f, {}, {0.0f, 0.0f, 1.0f}});
        simulator.simulate(trajectory, {}, {0.0f, 0.0f, -10.0f}, kind, nullptr);
        EXPECT_TRUE(trajectory.valid);
    }
}

}
