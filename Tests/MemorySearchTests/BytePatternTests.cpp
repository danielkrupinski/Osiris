#include <algorithm>
#include <array>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include <MemorySearch/BytePattern.h>

namespace
{

template <std::size_t N>
[[nodiscard]] constexpr auto createByteArray(const unsigned char(&bytes)[N])
{
    std::array<std::byte, N> arr;
    std::ranges::transform(bytes, arr.begin(), [](unsigned char c) { return std::byte{ c }; });
    return arr;
}

using namespace std::string_view_literals;

TEST(BytePatternTest, PatternMatchesMemoryWhenBytesAreTheSame) {
    EXPECT_TRUE(BytePattern{ "\xAB\xCD\xEF"sv }.matches(createByteArray({ 0xAB, 0xCD, 0xEF })));
}

TEST(BytePatternTest, PatternDoesNotMatchMemoryWhenBytesAreDifferent) {
    EXPECT_FALSE(BytePattern{ "\xAB\xCD\xAB"sv }.matches(createByteArray({ 0xAB, 0xCD, 0xEF })));
}

TEST(BytePatternTest, NullCharsInPatternDoesNotTerminateComparison) {
    EXPECT_FALSE(BytePattern{ "\xAB\x00\xEF\x00\x13"sv }.matches(createByteArray({ 0xAB, 0x00, 0xEF, 0x00, 0x12 })));
}

TEST(BytePatternTest, BytesOnWildcardPositionsAreIgnored) {
    const std::string pattern{ '\xAB', BytePattern::wildcardChar, '\xEF', BytePattern::wildcardChar, '\xFF' };
    EXPECT_TRUE(BytePattern{ pattern }.matches(createByteArray({ 0xAB, 0xCC, 0xEF, 0xDD, 0xFF })));
}

TEST(BytePatternTest, WildcardCharInMemoryDoesNotMatchEveryPatternChar) {
    EXPECT_FALSE(BytePattern{ "\xAB\xCC\xEF\xDD\xFF"sv }.matches(createByteArray({ 0xAB, BytePattern::wildcardChar, 0xEF, BytePattern::wildcardChar, 0xFF })));
}

}
