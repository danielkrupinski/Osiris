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

TEST_F(GrenadeSimulatorTraceTest, OptionalTraceFailureInvalidatesTrajectory)
{
    context.trace.push(TraceResult{0.5f, {}, {-1.0f, 0.0f, 0.0f}});
    context.trace.push(TraceResult{0.25f, {}, {0.0f, 1.0f, 0.0f}});
    context.trace.push({});
    simulator.simulate(trajectory, {}, {1000.0f, 0.0f, 0.0f}, cs2::GrenadeKind::HEGrenade, nullptr);
    EXPECT_FALSE(trajectory.valid);
    EXPECT_EQ(trajectory.pointsCount, 0);
}

TEST_F(GrenadeSimulatorTraceTest, ExplicitContinuationUsesAtMostThreeTracesPerSubstepAndTwoPrimaryContacts)
{
    context.trace.push(TraceResult{0.5f, {}, {-1.0f, 0.0f, 0.0f}});
    context.trace.push(TraceResult{0.25f, {}, {0.0f, 1.0f, 0.0f}});
    context.trace.push(TraceResult{1.0f, {}, {}});
    context.trace.push(TraceResult{0.5f, {}, {-1.0f, 0.0f, 0.0f}});
    context.trace.push(TraceResult{0.25f, {}, {0.0f, 1.0f, 0.0f}});
    context.trace.push(TraceResult{1.0f, {}, {}});
    context.trace.clearAfterScript();
    cs2::Vector position{};
    cs2::Vector velocity{1.0e12f, 0.0f, 0.0f};
    const auto result = GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade);
    EXPECT_TRUE(result.traceSucceeded);
    EXPECT_EQ(result.contactsCount, 2);
    EXPECT_EQ(context.trace.calls, 6);
}

TEST_F(GrenadeSimulatorTraceTest, FloorPrimaryUsesOneResponseAndOptionalPushOffWithExactTraceVectors)
{
    context.trace.push(TraceResult{0.5f, {}, {0.0f, 0.0f, 1.0f}});
    context.trace.push(TraceResult{0.0f, {}, {0.0f, 0.0f, 1.0f}});
    context.trace.push(TraceResult{1.0f, {}, {}});
    context.trace.clearAfterScript();
    cs2::Vector position{};
    cs2::Vector velocity{0.0f, 0.0f, -64.0f};
    const auto result = GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade);
    EXPECT_EQ(result.contactsCount, 1);
    EXPECT_EQ(context.trace.calls, 4);
    EXPECT_NEAR(context.trace.starts[1].z, 0.0f, 0.00001f);
    EXPECT_NEAR(context.trace.ends[1].z, 0.11694946f, 0.00001f);
    EXPECT_NEAR(context.trace.starts[2].z, 0.0f, 0.00001f);
    EXPECT_NEAR(context.trace.ends[2].z, 0.14819946f, 0.00001f);
    EXPECT_NEAR(velocity.z, 27.4390625f, 0.00001f);
}

TEST_F(GrenadeSimulatorTraceTest, OptionalContinuationThresholdIsStrictAndZeroMandatoryMoveIsTraced)
{
    context.trace.push(TraceResult{0.5f, {}, {-1.0f, 0.0f, 0.0f}});
    context.trace.push(TraceResult{0.5f, {}, {0.0f, 1.0f, 0.0f}});
    context.trace.clearAfterScript();
    cs2::Vector position{};
    cs2::Vector velocity{1000.0f, 0.0f, 0.0f};
    (void)GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade);
    EXPECT_EQ(context.trace.calls, 3); // Mandatory .5 is not optional.

    context.trace.calls = context.trace.resultCount = 0;
    context.trace.push(TraceResult{0.5f, {}, {-1.0f, 0.0f, 0.0f}});
    context.trace.push(TraceResult{0.499f, {}, {0.0f, 1.0f, 0.0f}});
    context.trace.push(TraceResult{1.0f, {}, {}});
    context.trace.clearAfterScript();
    position = {};
    velocity = {1000.0f, 0.0f, 0.0f};
    (void)GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade);
    EXPECT_EQ(context.trace.calls, 4);

    context.trace.calls = context.trace.resultCount = 0;
    context.trace.push(TraceResult{1.0f, {}, {}});
    position = {3.0f, 0.0f, 0.0f};
    velocity = {100.0f, 0.0f, 0.0f};
    const auto continuation = GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::continuePrimary(simulator, TraceResult{1.0f, position, {-1.0f, 0.0f, 0.0f}}, position, velocity, cs2::GrenadeKind::HEGrenade);
    EXPECT_TRUE(continuation.traceSucceeded);
    EXPECT_EQ(context.trace.calls, 1);
    EXPECT_EQ(context.trace.starts[0].x, 3.0f);
    EXPECT_EQ(context.trace.ends[0].x, 3.0f);
}

TEST_F(GrenadeSimulatorTraceTest, PrimaryFractionsUseFullAndTinyMandatoryDisplacements)
{
    context.trace.push(TraceResult{0.0f, {}, {-1.0f, 0.0f, 0.0f}});
    context.trace.push(TraceResult{1.0f, {}, {}});
    context.trace.clearAfterScript();
    cs2::Vector position{};
    cs2::Vector velocity{1000.0f, 0.0f, 0.0f};
    (void)GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade);
    EXPECT_NEAR(context.trace.ends[1].x - context.trace.starts[1].x, -3.51573467f, 0.00001f);

    context.trace.calls = context.trace.resultCount = 0;
    context.trace.push(TraceResult{0.999f, {}, {-1.0f, 0.0f, 0.0f}});
    context.trace.push(TraceResult{1.0f, {}, {}});
    context.trace.clearAfterScript();
    position = {};
    velocity = {1000.0f, 0.0f, 0.0f};
    (void)GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade);
    EXPECT_GT(Math::abs(context.trace.ends[1].x - context.trace.starts[1].x), 0.0f);
    EXPECT_NEAR(context.trace.ends[1].x - context.trace.starts[1].x, -0.00351569f, 0.00001f);
}

TEST_F(GrenadeSimulatorTraceTest, MandatoryFloorDoesNotReenterMolotovCallback)
{
    // Lower trace-provider callback re-entry is unknown, so a mandatory floor hit is not a second explicit impact.
    context.trace.push(TraceResult{0.5f, {}, {-1.0f, 0.0f, 0.0f}});
    context.trace.push(TraceResult{0.25f, {}, {0.0f, 0.0f, 1.0f}});
    context.trace.push(TraceResult{1.0f, {}, {}});
    context.trace.clearAfterScript();
    cs2::Vector position{};
    cs2::Vector velocity{1000.0f, 0.0f, -64.0f};
    const auto result = GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::Molotov);
    EXPECT_FALSE(result.impactDetonate);
    EXPECT_EQ(result.contactsCount, 1);

    context.trace.calls = context.trace.resultCount = 0;
    context.trace.push(TraceResult{0.5f, {}, {0.0f, 0.0f, 1.0f}});
    velocity = {1000.0f, 0.0f, -64.0f};
    EXPECT_TRUE(GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::Molotov).impactDetonate);
}

TEST_F(GrenadeSimulatorTraceTest, CapsRecordedPointsAndBounces)
{
    context.trace.clearAfterScript();
    simulator.simulate(trajectory, {}, {100.0f, 0.0f, 0.0f}, cs2::GrenadeKind::SmokeGrenade, nullptr);
    EXPECT_LE(trajectory.pointsCount, 500);
    EXPECT_LE(trajectory.bouncesCount, 20);
}

TEST_F(GrenadeSimulatorTraceTest, BounceLimitCountsOnlyPrimaryContactsAndCapsMarkers)
{
    for (int i = 0; i <= 20; ++i) {
        context.trace.push(TraceResult{0.5f, {}, {-1.0f, 0.0f, 0.0f}});
        context.trace.push(TraceResult{1.0f, {}, {}});
    }
    context.trace.clearAfterScript();
    simulator.simulate(trajectory, {}, {1.0e12f, 0.0f, 0.0f}, cs2::GrenadeKind::SmokeGrenade, nullptr);
    EXPECT_TRUE(trajectory.valid);
    EXPECT_EQ(trajectory.bouncesCount, 20);
    // Tick 11 terminates at 22 contacts, well before smoke's safety cap; its final stopped contact
    // requires no continuation trace, leaving 43 total trace calls.
    EXPECT_EQ(context.trace.calls, 43);
}

TEST_F(GrenadeSimulatorTraceTest, SmokeFreeFlightReachesSafetyCapWithoutExceedingPointStorage)
{
    context.trace.clearAfterScript();
    simulator.simulate(trajectory, {}, {100.0f, 0.0f, 0.0f}, cs2::GrenadeKind::SmokeGrenade, nullptr);
    EXPECT_TRUE(trajectory.valid);
    EXPECT_LE(trajectory.pointsCount, 500);
    EXPECT_NEAR(trajectory.endPos.x, 1803.125f, 0.001f); // 1154 * 100 / 64
    EXPECT_EQ(context.trace.calls, 2308);
}

TEST_F(GrenadeSimulatorTraceTest, CollisionFreeOuterStepUsesTwoSubstepsAndMatchesVerletKinematics)
{
    context.trace.clearAfterScript();
    cs2::Vector position{};
    cs2::Vector velocity{64.0f, 0.0f, 10.0f};
    const auto result = GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade);
    EXPECT_TRUE(result.traceSucceeded);
    EXPECT_FALSE(result.hit);
    EXPECT_EQ(context.trace.calls, 2);
    EXPECT_NEAR(position.x, 1.0f, 0.00001f);
    EXPECT_NEAR(position.z, 10.0f / 64.0f - 0.5f * 320.0f / (64.0f * 64.0f), 0.00001f);
    EXPECT_NEAR(velocity.z, 5.0f, 0.00001f);
}

TEST_F(GrenadeSimulatorTraceTest, ImpactDetonationStopsOnTheContactingSubstep)
{
    context.trace.push(TraceResult{0.5f, {}, {0.0f, 0.0f, 1.0f}});
    cs2::Vector position{};
    cs2::Vector velocity{100.0f, 0.0f, -64.0f};
    EXPECT_TRUE(GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::Molotov).impactDetonate);
    EXPECT_EQ(context.trace.calls, 1);

    context.trace.calls = context.trace.resultCount = 0;
    context.trace.push(TraceResult{1.0f, {}, {}});
    context.trace.push(TraceResult{0.5f, {}, {0.0f, 0.0f, 1.0f}});
    velocity = {100.0f, 0.0f, -64.0f};
    EXPECT_TRUE(GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::Molotov).impactDetonate);
    EXPECT_EQ(context.trace.calls, 2);
}

TEST_F(GrenadeSimulatorTraceTest, SecondSubstepFailureInvalidatesTrajectoryAndStoppedFirstSubstepStillAdvancesSecond)
{
    context.trace.push(TraceResult{1.0f, {}, {}});
    context.trace.push({});
    simulator.simulate(trajectory, {}, {64.0f, 0.0f, 0.0f}, cs2::GrenadeKind::HEGrenade, nullptr);
    EXPECT_FALSE(trajectory.valid);
    EXPECT_EQ(trajectory.pointsCount, 0);

    context.trace.calls = context.trace.resultCount = 0;
    context.trace.push(TraceResult{0.5f, {}, {0.0f, 0.0f, 1.0f}});
    context.trace.clearAfterScript();
    cs2::Vector position{};
    cs2::Vector velocity{0.0f, 0.0f, -10.0f};
    const auto result = GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade);
    EXPECT_TRUE(result.hit);
    EXPECT_EQ(context.trace.calls, 2);
    EXPECT_EQ(velocity.x, 0.0f);
    EXPECT_NEAR(velocity.z, -2.5f, 0.00001f);
    EXPECT_LT(position.z, 0.0f);
}

TEST_F(GrenadeSimulatorTraceTest, RecordsOnlyTwoPrimaryContactsInSubstepOrder)
{
    context.trace.push(TraceResult{0.5f, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}});
    context.trace.push(TraceResult{0.25f, {10.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}});
    context.trace.push(TraceResult{1.0f, {}, {}});
    context.trace.push(TraceResult{0.5f, {2.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}});
    context.trace.push(TraceResult{0.25f, {20.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}});
    context.trace.push(TraceResult{1.0f, {}, {}});
    cs2::Vector position{};
    cs2::Vector velocity{1.0e12f, 0.0f, 0.0f};
    const auto result = GrenadeSimulatorTestAccess<GrenadeSimulatorTestHookContext>::step(simulator, position, velocity, cs2::GrenadeKind::HEGrenade);
    ASSERT_EQ(result.contactsCount, 2);
    EXPECT_EQ(result.hitPos.x, 1.0f);
    EXPECT_EQ(context.trace.calls, 6);
    EXPECT_EQ(result.contacts[0].x, 1.0f);
    EXPECT_EQ(result.contacts[1].x, 2.0f);
}

TEST_F(GrenadeSimulatorTraceTest, SmokeAndDecoyDeployWhenCollisionStopsDisplacement)
{
    for (const auto kind : {cs2::GrenadeKind::SmokeGrenade, cs2::GrenadeKind::Decoy}) {
        context.trace.calls = context.trace.resultCount = 0;
        // Both movement substeps hit the same floor and stop at the outer start position.
        context.trace.push(TraceResult{0.5f, {}, {0.0f, 0.0f, 1.0f}});
        context.trace.push(TraceResult{0.5f, {}, {0.0f, 0.0f, 1.0f}});
        simulator.simulate(trajectory, {}, {0.0f, 0.0f, -10.0f}, kind, nullptr);
        EXPECT_TRUE(trajectory.valid);
        EXPECT_EQ(context.trace.calls, 2);
        EXPECT_EQ(trajectory.pointsCount, 1);
        EXPECT_NEAR(trajectory.endPos.x, 0.0f, 0.00001f);
        EXPECT_NEAR(trajectory.endPos.y, 0.0f, 0.00001f);
        EXPECT_NEAR(trajectory.endPos.z, 0.0f, 0.00001f);
    }
}

}
