#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>

#include <emmintrin.h>

#include <gtest/gtest.h>

#include <MemorySearch/PatternFinder.h>

namespace
{

template <std::size_t N>
[[nodiscard]] constexpr auto createByteArray(const unsigned char(&bytes)[N])
{
    std::array<std::byte, N> arr;
    std::ranges::transform(bytes, arr.begin(), [](unsigned char c) { return std::byte{ c }; });
    return arr;
}

template <std::size_t N>
[[nodiscard]] constexpr auto createByteArray()
{
    std::array<std::byte, N> arr;
    arr.fill(std::byte{ 0xCC });
    return arr;
}

using namespace std::literals::string_view_literals;

TEST(Utils_matchPatternTest, EmptyPatternMatchesEmptyBuffer) {
    EXPECT_TRUE(matchPattern({}, {}));
}

TEST(Utils_matchPatternTest, PatternMatchesMemoryWhenBytesAreTheSame) {
    EXPECT_TRUE(matchPattern(createByteArray({ 0xAB, 0xCD, 0xEF }), "\xAB\xCD\xEF"sv));
}

TEST(Utils_matchPatternTest, PatternDoesNotMatchMemoryWhenBytesAreDifferent) {
    EXPECT_FALSE(matchPattern(createByteArray({ 0xAB, 0xCD, 0xEF }), "\xAB\xCD\xAB"sv));
}

TEST(Utils_matchPatternTest, NullCharsInPatternDoesNotTerminateComparison) {
    EXPECT_FALSE(matchPattern(createByteArray({ 0xAB, 0x00, 0xEF, 0x00, 0x12 }), "\xAB\x00\xEF\x00\x13"sv));
}

TEST(Utils_matchPatternTest, BytesOnWildcardPositionsAreIgnored) {
    EXPECT_TRUE(matchPattern(createByteArray({ 0xAB, 0xCC, 0xEF, 0xDD, 0xFF }), std::string{ '\xAB', utils::wildcardChar, '\xEF', utils::wildcardChar, '\xFF' }));
}

TEST(Utils_matchPatternTest, WildcardCharInMemoryDoesNotMatchEveryPatternChar) {
    EXPECT_FALSE(matchPattern(createByteArray({ 0xAB, utils::wildcardChar, 0xEF, utils::wildcardChar, 0xFF }), "\xAB\xCC\xEF\xDD\xFF"sv));
}

TEST(Utils_PatternFinderTest, ShortPatternCanBeFound) {
    constexpr auto bytes = createByteArray({ 0xA0, 0xA1, 0xA2 });
    EXPECT_EQ(PatternFinder{ bytes }("\xA1"sv), &bytes[1]);
}

TEST(Utils_PatternFinderTest, ReturnsNullptrWhenByteBufferIsEmpty) {
    EXPECT_EQ(PatternFinder{ {} }("\x01\x02\x03\x04"sv), nullptr);
}

TEST(Utils_PatternFinderTest, ReturnsNullptrWhenPatternWasNotFound) {
    constexpr auto bytes = createByteArray({ 0xA0 });
    EXPECT_EQ(PatternFinder{ bytes }("\xAB\xCD\xEF"sv), nullptr);
}

TEST(Utils_PatternFinderTest, PatternCanBeFoundWithSIMD) {
    auto bytes = createByteArray<100>();
    bytes[30] = std::byte{ 0xDE };
    bytes[41] = std::byte{ 0xDE };
    bytes[42] = std::byte{ 0xAD };
    bytes[43] = std::byte{ 0x00 };
    bytes[44] = std::byte{ 0xC0 };
    bytes[45] = std::byte{ 0xDE };
    
    EXPECT_EQ(PatternFinder{ bytes }("\xDE\xAD?\xC0\xDE"sv), &bytes[41]);
}

}
