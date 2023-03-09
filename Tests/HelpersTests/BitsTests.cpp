#include <tuple>

#include <gtest/gtest.h>

#include <Helpers/Bits.h>

namespace
{

TEST(Helpers_CountrZero, PassingZeroRaisesDebugAssertion) {
    EXPECT_DEBUG_DEATH(std::ignore = bits::countrZero(0), "");
}

struct CountrZeroParam {
    unsigned int x;
    int firstSetBitIndex;
};

class Helpers_CountrZeroWithParam : public testing::TestWithParam<CountrZeroParam> {};

TEST_P(Helpers_CountrZeroWithParam, IndexOfFirstSetBitFromRightIsReturned) {
    EXPECT_EQ(bits::countrZero(GetParam().x), GetParam().firstSetBitIndex);
}

INSTANTIATE_TEST_SUITE_P(, Helpers_CountrZeroWithParam, testing::Values(
    CountrZeroParam{ 1, 0 },
    CountrZeroParam{ 0b100001000, 3 },
    CountrZeroParam{ 1u << 31, 31 }
));

}
