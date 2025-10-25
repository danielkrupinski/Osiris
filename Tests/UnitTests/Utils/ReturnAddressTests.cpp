#include <utility>

#include <gtest/gtest.h>

#include <Utils/ReturnAddress.h>

namespace
{

class ReturnAddressTest : public testing::TestWithParam<std::pair<void*, void*>> {};

TEST_P(ReturnAddressTest, SameReturnAddressesCompareEqual) {
    EXPECT_EQ(ReturnAddress{GetParam().first}, ReturnAddress{GetParam().first});
}

TEST_P(ReturnAddressTest, DifferentReturnAddressesDoNotCompareEqual) {
    EXPECT_NE(ReturnAddress{GetParam().first}, ReturnAddress{GetParam().second});
    EXPECT_NE(ReturnAddress{GetParam().second}, ReturnAddress{GetParam().first});
}

INSTANTIATE_TEST_SUITE_P(, ReturnAddressTest, testing::Values(
    std::pair{nullptr, reinterpret_cast<void*>(1234)},
    std::pair{reinterpret_cast<void*>(2048), reinterpret_cast<void*>(-1)},
    std::pair{reinterpret_cast<void*>(-1), nullptr}
));

}
