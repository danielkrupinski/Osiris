#include <array>
#include <cstddef>
#include <cstring>
#include <tuple>

#include <gtest/gtest.h>

#include <MemorySearch/PatternSearchResult.h>

namespace
{

TEST(PatternSearchResultDefaultConstructedTest, HasNullPointer) {
    EXPECT_EQ(PatternSearchResult{}.as<void*>(), nullptr);
}

TEST(PatternSearchResultDefaultConstructedTest, AddMethodDoesNothing) {
    EXPECT_EQ(PatternSearchResult{}.add(5).as<void*>(), nullptr);
}

TEST(PatternSearchResultDefaultConstructedTest, AbsMethodReturnsNullPointer) {
    EXPECT_EQ(PatternSearchResult{}.add(7).abs().as<void*>(), nullptr);
}

class PatternSearchResultTest : public testing::Test {
protected:
    std::array<std::byte, 50> runtimeMemory;
    std::array<std::byte, 60> foundPatternMemory;
};

class PatternSearchResultToAbsoluteTest : public PatternSearchResultTest, public testing::WithParamInterface<std::tuple<std::size_t, std::int32_t>> {
protected:
    static constexpr auto kRuntimeMemoryOffset{3};

    PatternSearchResultToAbsoluteTest() noexcept
    {
        const auto relativeOffset{getRelativeOffset()};
        std::memcpy(foundPatternMemory.data(), &relativeOffset, sizeof(relativeOffset));
    }

    [[nodiscard]] std::size_t getFoundPatternOffset() const noexcept
    {
        return std::get<0>(GetParam());
    }

    [[nodiscard]] std::int32_t getRelativeOffset() const noexcept
    {
        return std::get<1>(GetParam());
    }

    [[nodiscard]] PatternSearchResult patternSearchResult() const noexcept
    {
        return PatternSearchResult{runtimeMemory.data() + kRuntimeMemoryOffset, getFoundPatternOffset(), foundPatternMemory};
    }
};

TEST_P(PatternSearchResultToAbsoluteTest, ToAbsoluteReturnsCorrectAddress) {
    EXPECT_EQ(patternSearchResult().abs().as<void*>(), &runtimeMemory.at(kRuntimeMemoryOffset + getFoundPatternOffset() + sizeof(std::int32_t) + getRelativeOffset()));
}

INSTANTIATE_TEST_SUITE_P(, PatternSearchResultToAbsoluteTest,
    testing::Combine(testing::Values(0, 1, 5), testing::Values(-5, 0, 7)));

}
