#include <string_view>
#include <type_traits>

#include <gtest/gtest.h>

#include <MemorySearch/BytePatternLiteral.h>
#include <MemorySearch/BytePatternStorage.h>

namespace
{

using namespace std::string_view_literals;

TEST(BytePatternLiteralTest, LiteralReturnsPatternStorage) {
    constexpr auto patternStorage = "00 11 22 33 AA BB CC"_pat;
    constexpr auto expectedPattern = "\x00\x11\x22\x33\xAA\xBB\xCC"sv;

    static_assert(std::is_same_v<std::decay_t<decltype(patternStorage)>, BytePatternStorage<expectedPattern.size()>>);
    ASSERT_EQ(patternStorage.size, expectedPattern.size());
    const auto actualPattern = std::string_view{ patternStorage.pattern.data(), patternStorage.size };
    EXPECT_EQ(actualPattern, expectedPattern);
}

}
