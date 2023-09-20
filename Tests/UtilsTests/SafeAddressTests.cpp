#include <array>
#include <cstddef>
#include <cstring>

#include <gtest/gtest.h>

#include <Utils/SafeAddress.h>

namespace
{

class SafeAddress_AddressTest : public testing::TestWithParam<void*> {};

TEST_P(SafeAddress_AddressTest, AfterConstructionHasAddressPassedToTheConstructor) {
    EXPECT_EQ(SafeAddress{GetParam()}.as<void*>(), GetParam());
}

TEST_P(SafeAddress_AddressTest, ReturnsCorrectAddressWhenConvertedToPointer) {
    EXPECT_EQ(SafeAddress{GetParam()}.as<int*>(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(, SafeAddress_AddressTest, testing::Values(
    nullptr,
    reinterpret_cast<void*>(123),
    reinterpret_cast<void*>(-1)
));

TEST(SafeAddressTest, AddingPositiveOffsetToZeroAddressDoesNothing) {
    EXPECT_EQ(SafeAddress{nullptr}.add(1'000).as<void*>(), nullptr);
}

TEST(SafeAddressTest, AddingZeroOffsetToZeroAddressDoesNothing) {
    EXPECT_EQ(SafeAddress{nullptr}.add(0).as<void*>(), nullptr);
}

TEST(SafeAddressTest, AddingNegativeOffsetToZeroAddressDoesNothing) {
    EXPECT_EQ(SafeAddress{nullptr}.add(-1'000).as<void*>(), nullptr);
}

TEST(SafeAddressTest, AddingPositiveOffsetToNonZeroAddressWorks) {
    std::byte array[10];
    EXPECT_EQ(SafeAddress{&array}.add(5).as<void*>(), &array[5]);
}

TEST(SafeAddressTest, AddingZeroOffsetToNonZeroAddressDoesNothing) {
    std::byte array[10];
    EXPECT_EQ(SafeAddress{&array[4]}.add(0).as<void*>(), &array[4]);
}

TEST(SafeAddressTest, AddingNegativeOffsetToNonZeroAddressWorks) {
    std::byte array[10];
    EXPECT_EQ(SafeAddress{&array[7]}.add(-4).as<void*>(), &array[3]);
}

TEST(SafeAddressTest, CanBeDereferenced) {
    int* ptr1 = nullptr;
    int** ptr2 = &ptr1;
    int*** ptr3 = &ptr2;
    int**** ptr4 = &ptr3;

    EXPECT_EQ(SafeAddress{ptr4}.deref<2>().as<int**>(), &ptr1);
}

TEST(SafeAddressTest, DereferenceStopsWhenReachedZeroAddress) {
    int*** ptr3 = nullptr;
    int**** ptr4 = &ptr3;
    int***** ptr5 = &ptr4;

    EXPECT_EQ(SafeAddress{ptr5}.deref<10>().as<void*>(), nullptr);
}

TEST(SafeAddressTest, DereferencingZeroAddressDoesNothing) {
    EXPECT_EQ(SafeAddress{nullptr}.deref<2>().as<void*>(), nullptr);
    EXPECT_EQ(SafeAddress{nullptr}.deref().as<void*>(), nullptr);
}

TEST(SafeAddressTest, NonTemplateDerefMethodDereferencesOnce) {
    int* ptr1 = nullptr;
    int** ptr2 = &ptr1;
    int*** ptr3 = &ptr2;

    EXPECT_EQ(SafeAddress{ptr3}.deref().as<int**>(), &ptr1);
}

TEST(SafeAddressTest, ConvertingNullPointerToAbsoluteDoesNothing) {
    EXPECT_EQ(SafeAddress{nullptr}.abs().as<void*>(), nullptr);
}

class SafeAddressToAbsoluteTest : public testing::TestWithParam<std::int32_t> {};

TEST_P(SafeAddressToAbsoluteTest, RelativeAddressIsCorrectlyConvertedToAbsolute) {
    std::array<std::byte, 100> array;
    const std::int32_t offset = GetParam();
    constexpr auto offsetIndex = 30;
    std::memcpy(&array.at(offsetIndex), &offset, sizeof(offset));
    EXPECT_EQ(SafeAddress{&array.at(offsetIndex)}.abs().as<std::byte*>(), &array.at(offsetIndex + sizeof(offset) + offset));
}

INSTANTIATE_TEST_SUITE_P(, SafeAddressToAbsoluteTest, testing::Values(-12, 0, 50));

}
