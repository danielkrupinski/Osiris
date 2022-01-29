#include <limits>
#include <numbers>

#include <gtest/gtest.h>

#include "../Osiris/Helpers.h"

struct ValuesPair {
    long double degrees;
    long double radians;

    friend std::ostream& operator<<(std::ostream& os, const ValuesPair& pair)
    {
        return os << pair.degrees << "deg";
    }
};

constexpr auto pi = std::numbers::pi_v<long double>;
static const auto valuesToTest = testing::Values(
    ValuesPair{ -720.0l, -pi * 4 },
    ValuesPair{ -180.0l, -pi },
    ValuesPair{ -90.0l, -pi / 2 },
    ValuesPair{ 0.0l, 0.0l },
    ValuesPair{ 90.0l, pi / 2 },
    ValuesPair{ 180.0l, pi },
    ValuesPair{ 720.0l, pi * 4 }
);

class Deg2RadTest : public testing::TestWithParam<ValuesPair> {};

TEST_P(Deg2RadTest, ReturnsCorrectValueForFloat) {
    ASSERT_FLOAT_EQ(Helpers::deg2rad(static_cast<float>(GetParam().degrees)), static_cast<float>(GetParam().radians));
}

TEST_P(Deg2RadTest, ReturnsCorrectValueForDouble) {
    ASSERT_DOUBLE_EQ(Helpers::deg2rad(static_cast<double>(GetParam().degrees)), static_cast<double>(GetParam().radians));
}

TEST_P(Deg2RadTest, ReturnsCorrectValueForLongDouble) {
    ASSERT_DOUBLE_EQ(Helpers::deg2rad(static_cast<long double>(GetParam().degrees)), static_cast<long double>(GetParam().radians));
}

INSTANTIATE_TEST_SUITE_P(Helpers_Deg2Rad, Deg2RadTest, valuesToTest);

class Rad2DegTest : public testing::TestWithParam<ValuesPair> {};

TEST_P(Rad2DegTest, ReturnsCorrectValueForFloat) {
    ASSERT_FLOAT_EQ(Helpers::rad2deg(static_cast<float>(GetParam().radians)), static_cast<float>(GetParam().degrees));
}

TEST_P(Rad2DegTest, ReturnsCorrectValueForDouble) {
    ASSERT_DOUBLE_EQ(Helpers::rad2deg(static_cast<double>(GetParam().radians)), static_cast<double>(GetParam().degrees));
}

TEST_P(Rad2DegTest, ReturnsCorrectValueForLongDouble) {
    ASSERT_DOUBLE_EQ(Helpers::rad2deg(static_cast<long double>(GetParam().radians)), static_cast<long double>(GetParam().degrees));
}

INSTANTIATE_TEST_SUITE_P(Helpers_Rad2Deg, Rad2DegTest, valuesToTest);
