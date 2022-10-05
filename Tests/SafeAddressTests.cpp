#include <limits>

#include <gtest/gtest.h>

#include <SafeAddress.h>

namespace
{

constexpr auto minAddress = (std::numeric_limits<std::uintptr_t>::min)();
constexpr auto maxAddress = (std::numeric_limits<std::uintptr_t>::max)();

class SafeAddress_AddressTest : public testing::TestWithParam<std::uintptr_t> {};

TEST_P(SafeAddress_AddressTest, AfterConstructionHasAddressPassedToTheConstructor) {
    EXPECT_EQ(SafeAddress{ GetParam() }.get(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(, SafeAddress_AddressTest, testing::Values(minAddress, 123, maxAddress));

TEST(SafeAddressTest, AddingPositiveOffsetToZeroAddressDoesNothing) {
    EXPECT_EQ(SafeAddress{ 0 }.add(1'000).get(), 0);
}

TEST(SafeAddressTest, AddingZeroOffsetToZeroAddressDoesNothing) {
    EXPECT_EQ(SafeAddress{ 0 }.add(0).get(), 0);
}

TEST(SafeAddressTest, AddingNegativeOffsetToZeroAddressDoesNothing) {
    EXPECT_EQ(SafeAddress{ 0 }.add(-1'000).get(), 0);
}

TEST(SafeAddressTest, AddingZeroOffsetToAnAddressDoesNothing) {
    EXPECT_EQ(SafeAddress{ 123'456 }.add(0).get(), 123'456);
}

TEST(SafeAddressTest, AddingPositiveOffsetCanWrapAround) {
    EXPECT_EQ(SafeAddress{ maxAddress }.add(1).get(), minAddress);
}

TEST(SafeAddressTest, AddingNegativeOffsetCanWrapAround) {
    EXPECT_EQ(SafeAddress{ 10 }.add(-11).get(), maxAddress);
}

TEST(SafeAddressTest, CanBeDereferenced) {
    int* ptr1 = nullptr;
    int** ptr2 = &ptr1;
    int*** ptr3 = &ptr2;
    int**** ptr4 = &ptr3;

    EXPECT_EQ(SafeAddress{ std::uintptr_t(ptr4) }.deref<2>().get(), std::uintptr_t(&ptr1));
}

TEST(SafeAddressTest, DereferenceStopsWhenReachedZeroAddress) {
    int*** ptr3 = nullptr;
    int**** ptr4 = &ptr3;
    int***** ptr5 = &ptr4;

    EXPECT_EQ(SafeAddress{ std::uintptr_t(ptr5) }.deref<10>().get(), 0);
}

}
