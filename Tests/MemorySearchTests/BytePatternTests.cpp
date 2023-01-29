#include <algorithm>
#include <array>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include <MemorySearch/BytePattern.h>
#include <MemorySearch/BytePatternLiteral.h>

namespace
{

template <std::size_t N>
[[nodiscard]] constexpr auto createByteArray(const unsigned char(&bytes)[N])
{
    std::array<std::byte, N> arr;
    std::ranges::transform(bytes, arr.begin(), [](unsigned char c) { return std::byte{ c }; });
    return arr;
}

TEST(BytePatternTest, PatternMatchesMemoryWhenBytesAreTheSame) {
    constexpr auto pattern = "AB CD EF"_pat;
    EXPECT_TRUE(BytePattern{ pattern }.matches(createByteArray({ 0xAB, 0xCD, 0xEF })));
}

TEST(BytePatternTest, PatternDoesNotMatchMemoryWhenBytesAreDifferent) {
    constexpr auto pattern = "AB CD AB"_pat;
    EXPECT_FALSE(BytePattern{ pattern }.matches(createByteArray({ 0xAB, 0xCD, 0xEF })));
}

TEST(BytePatternTest, NullCharsInPatternDoesNotTerminateComparison) {
    constexpr auto pattern = "AB 00 EF 00 13"_pat;
    EXPECT_FALSE(BytePattern{ pattern }.matches(createByteArray({ 0xAB, 0x00, 0xEF, 0x00, 0x12 })));
}

TEST(BytePatternTest, BytesOnWildcardPositionsAreIgnored) {
    constexpr auto pattern = "AB ? EF ? FF"_pat;
    EXPECT_TRUE(BytePattern{ pattern }.matches(createByteArray({ 0xAB, 0xCC, 0xEF, 0xDD, 0xFF })));
}

TEST(BytePatternTest, WildcardCharInMemoryDoesNotMatchEveryPatternChar) {
    constexpr auto pattern = "AB CC EF DD FF"_pat;
    EXPECT_FALSE(BytePattern{ pattern }.matches(createByteArray({ 0xAB, BytePattern::wildcardChar, 0xEF, BytePattern::wildcardChar, 0xFF })));
}

}
