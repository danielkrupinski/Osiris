#include <bit>
#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <gtest/gtest.h>

#include <Platform/Macros/IsPlatform.h>
#include <GameClient/EngineTrace/EngineTrace.h>

#if defined(_WIN64)
namespace {

enum class MissingDependency { None, Builder, TraceShape, ManagerStorage, Holder, Filter };

struct CallState {
    MissingDependency missing{MissingDependency::None};
    void* holder{};
    bool traceReturnsCollision{};
    float fraction{1.0f};
    cs2::Vector endPos{1.0f, 2.0f, 3.0f};
    cs2::Vector normal{};
    bool descriptorWasZero{};
    bool filterWasZero{};
    engine_trace::Bounds6f bounds{};
    void* filterSkipEntity{};
    std::uint64_t filterMask{};
    std::uint8_t filterType{};
    std::uint8_t collisionGroup{};
    void* traceHolder{};
    cs2::Vector traceStart{};
    cs2::Vector traceEnd{};
    bool mutateManagerStorageDuringFilter{};
    std::int32_t endPositionOffset{32};
    std::int32_t normalOffset{48};
    std::int32_t fractionOffset{16};
};

CallState callState{};
void* managerHolder{};

template <typename T>
void writeOutput(engine_trace::OutputStorage* output, std::size_t offset, T value) noexcept
{
    const auto bytes = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
    for (std::size_t i = 0; i < sizeof(T); ++i)
        output->storage[offset + i] = bytes[i];
}

void shapeBuilder(void* descriptor, const engine_trace::Bounds6f* bounds) noexcept
{
    const auto* bytes = static_cast<const std::byte*>(descriptor);
    callState.descriptorWasZero = true;
    for (std::size_t i = 0; i < engine_trace::kDescriptorCapacity; ++i)
        callState.descriptorWasZero = callState.descriptorWasZero && bytes[i] == std::byte{};
    callState.bounds = *bounds;
}

void* initFilter(void* filter, void* skipEntity, std::uint64_t mask, std::uint8_t filterType, std::uint8_t collisionGroup) noexcept
{
    const auto* bytes = static_cast<const std::byte*>(filter);
    callState.filterWasZero = true;
    for (std::size_t i = 0; i < engine_trace::kFilterCapacity; ++i)
        callState.filterWasZero = callState.filterWasZero && bytes[i] == std::byte{};
    callState.filterSkipEntity = skipEntity;
    callState.filterMask = mask;
    callState.filterType = filterType;
    callState.collisionGroup = collisionGroup;
    if (callState.mutateManagerStorageDuringFilter)
        managerHolder = nullptr;
    return filter;
}

bool traceShape(void* holder, const void*, const cs2::Vector* start, const cs2::Vector* end, void*, void* output) noexcept
{
    callState.traceHolder = holder;
    callState.traceStart = *start;
    callState.traceEnd = *end;
    auto* traceOutput = static_cast<engine_trace::OutputStorage*>(output);
    writeOutput(traceOutput, 16, callState.fraction);
    writeOutput(traceOutput, 32, callState.endPos);
    writeOutput(traceOutput, 48, callState.normal);
    return callState.traceReturnsCollision;
}

struct PatternResults {
    template <typename T>
    auto get() const noexcept
    {
        if constexpr (std::is_same_v<T, ShapeBuilderFunctionPointer>)
            return callState.missing == MissingDependency::Builder ? nullptr : &shapeBuilder;
        else if constexpr (std::is_same_v<T, TraceShapeFunctionPointer>)
            return callState.missing == MissingDependency::TraceShape ? nullptr : &traceShape;
        else if constexpr (std::is_same_v<T, GameTraceManagerStoragePointer>)
            return callState.missing == MissingDependency::ManagerStorage ? nullptr : &managerHolder;
        else if constexpr (std::is_same_v<T, InitFilterFunctionPointer>)
            return callState.missing == MissingDependency::Filter ? nullptr : &initFilter;
        else if constexpr (std::is_same_v<T, CGameTraceEndPositionOffset>)
            return FieldOffset<engine_trace::OutputStorage, cs2::Vector, std::int32_t>{callState.endPositionOffset};
        else if constexpr (std::is_same_v<T, CGameTraceNormalOffset>)
            return FieldOffset<engine_trace::OutputStorage, cs2::Vector, std::int32_t>{callState.normalOffset};
        else
            return FieldOffset<engine_trace::OutputStorage, float, std::int32_t>{callState.fractionOffset};
    }
};

struct HookContext {
    [[nodiscard]] const PatternResults& patternSearchResults() const noexcept { return results; }
    PatternResults results{};
};

class EngineTraceTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        callState = {};
        managerHolder = &callState;
        callState.holder = managerHolder;
    }
};

TEST_F(EngineTraceTest, FailsClosedForEveryMissingPrerequisite)
{
    HookContext hookContext;
    EngineTrace<HookContext> trace{hookContext};
    for (const auto dependency : {MissingDependency::Builder, MissingDependency::TraceShape, MissingDependency::ManagerStorage, MissingDependency::Holder, MissingDependency::Filter}) {
        callState.missing = dependency;
        if (dependency == MissingDependency::Holder)
            managerHolder = nullptr;
        EXPECT_FALSE(trace.traceGrenadeHull({}, {}, nullptr).hasValue());
        managerHolder = callState.holder;
    }
}

TEST_F(EngineTraceTest, SnapshotsManagerBeforeFilterAndUsesSelectedMaskRegression)
{
    HookContext hookContext;
    EngineTrace<HookContext> trace{hookContext};
    void* skipEntity = reinterpret_cast<void*>(0x1234);
    const auto result = trace.traceGrenadeHull({10.0f, 20.0f, 30.0f}, {40.0f, 50.0f, 60.0f}, skipEntity);

    ASSERT_TRUE(result.hasValue());
    EXPECT_EQ(result.value().fraction, 1.0f);
    EXPECT_TRUE(callState.descriptorWasZero);
    EXPECT_TRUE(callState.filterWasZero);
    EXPECT_EQ(callState.bounds.mins.x, -2.0f);
    EXPECT_EQ(callState.bounds.maxs.z, 2.0f);
    EXPECT_EQ(callState.filterSkipEntity, skipEntity);
    // Regression coverage for the selected project mask, not native grenade-mask parity.
    EXPECT_EQ(callState.filterMask, MASK_GRENADE);
    EXPECT_EQ(callState.filterType, 4);
    EXPECT_EQ(callState.collisionGroup, 7);
    EXPECT_EQ(callState.traceStart.x, 10.0f);
    EXPECT_EQ(callState.traceStart.z, 30.0f);
    EXPECT_EQ(callState.traceEnd.x, 40.0f);
    EXPECT_EQ(callState.traceEnd.z, 60.0f);

    callState.mutateManagerStorageDuringFilter = true;
    ASSERT_TRUE(trace.traceGrenadeHull({}, {}, nullptr).hasValue());
    EXPECT_EQ(managerHolder, nullptr);
    EXPECT_EQ(callState.traceHolder, callState.holder);
}

TEST_F(EngineTraceTest, ReturnsCollisionAndRejectsInvalidCollisionOutput)
{
    HookContext hookContext;
    EngineTrace<HookContext> trace{hookContext};
    callState.traceReturnsCollision = true;
    callState.fraction = 0.5f;
    callState.normal = {0.0f, 0.0f, 1.0f};
    ASSERT_TRUE(trace.traceGrenadeHull({}, {}, nullptr).hasValue());

    callState.traceReturnsCollision = false;
    ASSERT_TRUE(trace.traceGrenadeHull({}, {}, nullptr).hasValue());
    callState.fraction = 1.0f;
    callState.normal = {};
    callState.traceReturnsCollision = true;
    ASSERT_TRUE(trace.traceGrenadeHull({}, {}, nullptr).hasValue());

    callState.fraction = 0.5f;
    callState.normal = {};
    EXPECT_FALSE(trace.traceGrenadeHull({}, {}, nullptr).hasValue());
    callState.normal = {0.0f, 0.0f, 1.0f};
    callState.fraction = std::bit_cast<float>(0x7F800000u);
    EXPECT_FALSE(trace.traceGrenadeHull({}, {}, nullptr).hasValue());
    callState.fraction = 1.1f;
    EXPECT_FALSE(trace.traceGrenadeHull({}, {}, nullptr).hasValue());
}

TEST_F(EngineTraceTest, FailsClosedForInvalidDynamicOffsets)
{
    HookContext hookContext;
    EngineTrace<HookContext> trace{hookContext};

    callState.endPositionOffset = 2;
    EXPECT_FALSE(trace.traceGrenadeHull({}, {}, nullptr).hasValue());
    callState.endPositionOffset = 32;
    callState.normalOffset = 32;
    EXPECT_FALSE(trace.traceGrenadeHull({}, {}, nullptr).hasValue());
    callState.normalOffset = 48;
    callState.fractionOffset = 0xC0;
    EXPECT_FALSE(trace.traceGrenadeHull({}, {}, nullptr).hasValue());
}

}
#endif
