#include <tuple>

#include <gtest/gtest.h>

#include <Utils/Bits.h>

namespace
{

TEST(CountrZero, PassingZeroRaisesDebugAssertion) {
    EXPECT_DEBUG_DEATH(std::ignore = bits::countrZero(0), "");
}

struct CountrZeroParam {
    unsigned int x;
    int firstSetBitIndex;
};

class CountrZeroWithParam : public testing::TestWithParam<CountrZeroParam> {};

TEST_P(CountrZeroWithParam, IndexOfFirstSetBitFromRightIsReturned) {
    EXPECT_EQ(bits::countrZero(GetParam().x), GetParam().firstSetBitIndex);
}

INSTANTIATE_TEST_SUITE_P(, CountrZeroWithParam, testing::Values(
    CountrZeroParam{ 1, 0 },
    CountrZeroParam{ 0b100001000, 3 },
    CountrZeroParam{ 1u << 31, 31 }
));

}
