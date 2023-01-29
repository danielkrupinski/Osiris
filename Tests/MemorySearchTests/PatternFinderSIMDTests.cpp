#include <array>
#include <cstddef>

#include <gtest/gtest.h>

#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/PatternFinderSIMD.h>

namespace
{

constexpr auto dummyPattern = "AA BB"_pat;

TEST(PatternFinderSIMD_NoBytesTest, NotCheckedBytesAreEmptySpan) {
    PatternFinderSIMD finder{ {}, dummyPattern };
    EXPECT_TRUE(finder.getNotCheckedBytes().empty());
}

TEST(PatternFinderSIMD_NoBytesTest, FinderReturnsNullptr) {
    PatternFinderSIMD finder{ {}, dummyPattern };
    EXPECT_EQ(finder(), nullptr);
}

TEST(PatternFinderSIMDTest, NoBytesAreCheckedUntilFinderIsInvoked) {
    std::array<std::byte, 1000> bytes{};
    constexpr auto pattern = "AA"_pat;
    PatternFinderSIMD finder{ bytes, pattern };
    const auto notCheckedBytes = finder.getNotCheckedBytes();
    ASSERT_EQ(notCheckedBytes.size(), bytes.size());
    EXPECT_EQ(&notCheckedBytes.front(), &bytes.front());
    EXPECT_EQ(&notCheckedBytes.back(), &bytes.back());
}

TEST(PatternFinderSIMDTest, OneBytePatternCanBeMatched) {
    std::array<std::byte, 64> bytes{};
    bytes[20] = std::byte{ 0x12 };

    constexpr auto pattern = "12"_pat;
    PatternFinderSIMD finder{ bytes, pattern };
    EXPECT_EQ(finder(), &bytes[20]);
}

TEST(PatternFinderSIMDTest, FirstAndLastCharOfPatternAreCheckedCorrectly) {
    std::array<std::byte, 64> bytes{};
    bytes[20] = std::byte{ 0xBB };
    bytes[21] = std::byte{ 0xBB };
    bytes[22] = std::byte{ 0xAA };
    bytes[23] = std::byte{ 0xAA };
    bytes[24] = std::byte{ 0xAA };
    bytes[25] = std::byte{ 0xBB };

    constexpr auto pattern = "AA BB"_pat;
    PatternFinderSIMD finder{ bytes, pattern };
    EXPECT_EQ(finder(), &bytes[24]);
}

TEST(PatternFinderSIMDTest, PatternWithoutFirstAndLastCharIsCheckedCorrectly) {
    std::array<std::byte, 64> bytes{};
    bytes[20] = std::byte{ 0xAA };
    bytes[21] = std::byte{ 0x33 };
    bytes[22] = std::byte{ 0xBB };
    bytes[23] = std::byte{ 0xAA };
    bytes[24] = std::byte{ 0x12 };
    bytes[25] = std::byte{ 0xBB };

    constexpr auto pattern = "AA 12 BB"_pat;
    PatternFinderSIMD finder{ bytes, pattern };
    EXPECT_EQ(finder(), &bytes[23]);
}

TEST(PatternFinderSIMDTest, BytesInTheLastXmmwordAreChecked) {
    std::array<std::byte, 64> bytes{};
    bytes[62] = std::byte{ 0xAA };
    constexpr auto pattern = "AA"_pat;
    PatternFinderSIMD finder{ bytes, pattern };
    EXPECT_EQ(finder(), &bytes[62]);
}

}
