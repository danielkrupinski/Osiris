#include <algorithm>
#include <array>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include <MemorySearch/BytePattern.h>
#include <MemorySearch/PatternStringWildcard.h>

namespace
{

using namespace std::string_view_literals;

template <std::size_t N>
[[nodiscard]] constexpr auto createByteArray(const unsigned char(&bytes)[N])
{
    std::array<std::byte, N> arr;
    std::ranges::transform(bytes, arr.begin(), [](unsigned char c) { return std::byte{c}; });
    return arr;
}

TEST(BytePatternWithoutWildcardTest, PatternMatchesMemoryWhenBytesAreTheSame) {
    EXPECT_TRUE(BytePattern{"\xAB\xCD\xEF"sv}.matches(createByteArray({ 0xAB, 0xCD, 0xEF })));
}

TEST(BytePatternWithoutWildcardTest, PatternDoesNotMatchMemoryWhenBytesAreDifferent) {
    EXPECT_FALSE(BytePattern{"\xAB\xCD\xAB"sv}.matches(createByteArray({ 0xAB, 0xCD, 0xEF })));
}

TEST(BytePatternWithoutWildcardTest, NullCharsInPatternDoesNotTerminateComparison) {
    EXPECT_FALSE(BytePattern{"\xAB\x00\xEF\x00\x13"sv}.matches(createByteArray({ 0xAB, 0x00, 0xEF, 0x00, 0x12 })));
}

class BytePatternWithWildcardTest : public testing::TestWithParam<char> {
protected:
    [[nodiscard]] auto makePattern(std::string_view pattern) noexcept
    {
        return BytePattern{pattern, GetParam()};
    }
};

TEST_P(BytePatternWithWildcardTest, PatternMatchesMemoryWhenBytesAreTheSame) {
    EXPECT_TRUE(makePattern("\xAB\xCD\xEF"sv).matches(createByteArray({ 0xAB, 0xCD, 0xEF })));
}

TEST_P(BytePatternWithWildcardTest, PatternDoesNotMatchMemoryWhenBytesAreDifferent) {
    EXPECT_FALSE(makePattern("\xAB\xCD\xAB"sv).matches(createByteArray({ 0xAB, 0xCD, 0xEF })));
}

TEST_P(BytePatternWithWildcardTest, NullCharsInPatternDoesNotTerminateComparison) {
    EXPECT_FALSE(makePattern("\xAB\x00\xEF\x00\x13"sv).matches(createByteArray({ 0xAB, 0x00, 0xEF, 0x00, 0x12 })));
}

TEST_P(BytePatternWithWildcardTest, BytesOnWildcardPositionsAreIgnored) {
    const auto pattern = std::string{"\xAB"} + GetParam() + "\xEF" + GetParam() + "\xFF";
    EXPECT_TRUE(makePattern(pattern).matches(createByteArray({ 0xAB, 0xCC, 0xEF, 0xDD, 0xFF })));
}

TEST_P(BytePatternWithWildcardTest, WildcardCharInMemoryDoesNotMatchEveryPatternChar) {
    EXPECT_FALSE(makePattern("\xAB\xCC\xEF\xDD\xFF"sv).matches(createByteArray({ 0xAB, static_cast<unsigned char>(GetParam()), 0xEF, static_cast<unsigned char>(GetParam()), 0xFF })));
}

INSTANTIATE_TEST_SUITE_P(, BytePatternWithWildcardTest, testing::Values('?', '.'));

}
