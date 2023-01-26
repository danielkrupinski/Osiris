#include <gtest/gtest.h>

#include <MemorySearch/BytePatternConverter.h>

namespace
{

TEST(BytePatternConverterTest, ValidPatternIsFullyConvertedAndNoErrorIsReturned) {
    BytePatternConverter converter{ "8B CE E8 ? ? ? ? 02 C0" };
    const auto [converted, error] = converter();
    EXPECT_EQ(converted, "\x8B\xCE\xE8????\x02\xC0");
    EXPECT_EQ(error, BytePatternConverterError::NoError);
}

TEST(BytePatternConverterTest, EmptyPatternResultsInUnexpectedCharError) {
    BytePatternConverter converter{ "" };
    const auto [converted, error] = converter();
    EXPECT_EQ(converted, "");
    EXPECT_EQ(error, BytePatternConverterError::UnexpectedChar);
}

TEST(BytePatternConverterTest, PatternIsConvertedUntilAnErrorHappens) {
    BytePatternConverter converter{ "AA BB x" };
    const auto [converted, error] = converter();
    EXPECT_EQ(converted, "\xAA\xBB");
    EXPECT_EQ(error, BytePatternConverterError::UnexpectedChar);
}

TEST(BytePatternConverterTest, PatternCannotStartWithWildcard) {
    BytePatternConverter converter{ "? AA BB" };
    const auto [converted, error] = converter();
    EXPECT_EQ(converted, "");
    EXPECT_EQ(error, BytePatternConverterError::StartsWithWildcard);
}

TEST(BytePatternConverterTest, PatternCannotEndWithWildcard) {
    BytePatternConverter converter{ "AA BB ?" };
    const auto [converted, error] = converter();
    EXPECT_EQ(converted, "\xAA\xBB");
    EXPECT_EQ(error, BytePatternConverterError::EndsWithWildcard);
}

TEST(BytePatternConverterTest, NumericValueOfWildcardCharCannotBeUsed) {
    static_assert(BytePatternConverter<1>::wildcardChar == 0x3F);

    BytePatternConverter converter{ "AA BB 3F CC" };
    const auto [converted, error] = converter();
    EXPECT_EQ(converted, "\xAA\xBB");
    EXPECT_EQ(error, BytePatternConverterError::WildcardUsedAsByte);
}

}
