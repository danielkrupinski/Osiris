#include <array>
#include <cstddef>
#include <cstring>

#include <gtest/gtest.h>

#include <MemorySearch/PatternSearchResult.h>

namespace
{

class PatternSearchResult_AddressTest : public testing::TestWithParam<void*> {};

TEST_P(PatternSearchResult_AddressTest, AfterConstructionHasAddressPassedToTheConstructor) {
    EXPECT_EQ(PatternSearchResult{GetParam()}.as<void*>(), GetParam());
}

TEST_P(PatternSearchResult_AddressTest, ReturnsCorrectAddressWhenConvertedToPointer) {
    EXPECT_EQ(PatternSearchResult{GetParam()}.as<int*>(), GetParam());
}

INSTANTIATE_TEST_SUITE_P(, PatternSearchResult_AddressTest, testing::Values(
    nullptr,
    reinterpret_cast<void*>(123),
    reinterpret_cast<void*>(-1)
));

TEST(PatternSearchResultTest, AddingPositiveOffsetToZeroAddressDoesNothing) {
    EXPECT_EQ(PatternSearchResult{nullptr}.add(1'000).as<void*>(), nullptr);
}

TEST(PatternSearchResultTest, AddingZeroOffsetToZeroAddressDoesNothing) {
    EXPECT_EQ(PatternSearchResult{nullptr}.add(0).as<void*>(), nullptr);
}

TEST(PatternSearchResultTest, AddingNegativeOffsetToZeroAddressDoesNothing) {
    EXPECT_EQ(PatternSearchResult{nullptr}.add(-1'000).as<void*>(), nullptr);
}

TEST(PatternSearchResultTest, AddingPositiveOffsetToNonZeroAddressWorks) {
    std::byte array[10];
    EXPECT_EQ(PatternSearchResult{&array}.add(5).as<void*>(), &array[5]);
}

TEST(PatternSearchResultTest, AddingZeroOffsetToNonZeroAddressDoesNothing) {
    std::byte array[10];
    EXPECT_EQ(PatternSearchResult{&array[4]}.add(0).as<void*>(), &array[4]);
}

TEST(PatternSearchResultTest, AddingNegativeOffsetToNonZeroAddressWorks) {
    std::byte array[10];
    EXPECT_EQ(PatternSearchResult{&array[7]}.add(-4).as<void*>(), &array[3]);
}

TEST(PatternSearchResultTest, CanBeDereferenced) {
    int* ptr1 = nullptr;
    int** ptr2 = &ptr1;
    int*** ptr3 = &ptr2;
    int**** ptr4 = &ptr3;

    EXPECT_EQ(PatternSearchResult{ptr4}.deref<2>().as<int**>(), &ptr1);
}

TEST(PatternSearchResultTest, DereferenceStopsWhenReachedZeroAddress) {
    int*** ptr3 = nullptr;
    int**** ptr4 = &ptr3;
    int***** ptr5 = &ptr4;

    EXPECT_EQ(PatternSearchResult{ptr5}.deref<10>().as<void*>(), nullptr);
}

TEST(PatternSearchResultTest, DereferencingZeroAddressDoesNothing) {
    EXPECT_EQ(PatternSearchResult{nullptr}.deref<2>().as<void*>(), nullptr);
    EXPECT_EQ(PatternSearchResult{nullptr}.deref().as<void*>(), nullptr);
}

TEST(PatternSearchResultTest, NonTemplateDerefMethodDereferencesOnce) {
    int* ptr1 = nullptr;
    int** ptr2 = &ptr1;
    int*** ptr3 = &ptr2;

    EXPECT_EQ(PatternSearchResult{ptr3}.deref().as<int**>(), &ptr1);
}

TEST(PatternSearchResultTest, ConvertingNullPointerToAbsoluteDoesNothing) {
    EXPECT_EQ(PatternSearchResult{nullptr}.abs().as<void*>(), nullptr);
}

class PatternSearchResultToAbsoluteTest : public testing::TestWithParam<std::int32_t> {};

TEST_P(PatternSearchResultToAbsoluteTest, RelativeAddressIsCorrectlyConvertedToAbsolute) {
    std::array<std::byte, 100> array;
    const std::int32_t offset = GetParam();
    constexpr auto offsetIndex = 30;
    std::memcpy(&array.at(offsetIndex), &offset, sizeof(offset));
    EXPECT_EQ(PatternSearchResult{&array.at(offsetIndex)}.abs().as<std::byte*>(), &array.at(offsetIndex + sizeof(offset) + offset));
}

INSTANTIATE_TEST_SUITE_P(, PatternSearchResultToAbsoluteTest, testing::Values(-12, 0, 50));

}
