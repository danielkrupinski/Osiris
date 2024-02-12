#include <gtest/gtest.h>

#include <MemorySearch/BytePatternConverter.h>
#include <MemorySearch/PatternStringWildcard.h>

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

TEST(BytePatternConverterTest, PatternCanStartWithWildcard) {
    BytePatternConverter converter{"? AA BB"};
    const auto [converted, error] = converter();
    EXPECT_EQ(converted, "?\xAA\xBB");
    EXPECT_EQ(error, BytePatternConverterError::NoError);
}

TEST(BytePatternConverterTest, PatternCanEndWithWildcard) {
    BytePatternConverter converter{"AA BB ?"};
    const auto [converted, error] = converter();
    EXPECT_EQ(converted, "\xAA\xBB?");
    EXPECT_EQ(error, BytePatternConverterError::NoError);
}

TEST(BytePatternConverterTest, NumericValueOfWildcardCharCannotBeUsed) {
    static_assert(kPatternStringWildcard == 0x3F);

    BytePatternConverter converter{ "AA BB 3F CC" };
    const auto [converted, error] = converter();
    EXPECT_EQ(converted, "\xAA\xBB");
    EXPECT_EQ(error, BytePatternConverterError::WildcardUsedAsByte);
}

}
