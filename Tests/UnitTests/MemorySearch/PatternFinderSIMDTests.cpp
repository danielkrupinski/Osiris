#include <array>
#include <cstddef>

#include <gtest/gtest.h>

#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/PatternFinderSIMD.h>

namespace
{

constexpr BytePattern dummyPattern{"\xAA\xBB"};

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
    PatternFinderSIMD finder{bytes, BytePattern{"\xAA"}};
    const auto notCheckedBytes = finder.getNotCheckedBytes();
    ASSERT_EQ(notCheckedBytes.size(), bytes.size());
    EXPECT_EQ(&notCheckedBytes.front(), &bytes.front());
    EXPECT_EQ(&notCheckedBytes.back(), &bytes.back());
}

TEST(PatternFinderSIMDTest, OneBytePatternCanBeMatched) {
    std::array<std::byte, 64> bytes{};
    bytes[20] = std::byte{ 0x12 };

    PatternFinderSIMD finder{bytes, BytePattern{"\x12"}};
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

    PatternFinderSIMD finder{bytes, BytePattern{"\xAA\xBB"}};
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

    PatternFinderSIMD finder{bytes, BytePattern{"\xAA\x12\xBB"}};
    EXPECT_EQ(finder(), &bytes[23]);
}

TEST(PatternFinderSIMDTest, BytesInTheLastXmmwordAreChecked) {
    std::array<std::byte, 64> bytes{};
    bytes[62] = std::byte{ 0xAA };
    PatternFinderSIMD finder{bytes, BytePattern{"\xAA"}};
    EXPECT_EQ(finder(), &bytes[62]);
}

}
