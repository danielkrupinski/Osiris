#include <array>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <range/v3/view/map.hpp>

#include <SortFilter.h>

namespace
{

TEST(SortFilterTest, HasNoItemsWhenConstructedFromEmptySpan) {
    ASSERT_EQ(SortFilter<int>{{}}.totalItemCount(), 0);
}

TEST(SortFilterTest, GetItemsReturnsEmptyRangeWhenConstructedFromEmptySpan) {
    ASSERT_TRUE(SortFilter<int>{{}}.getItems().empty());
}

constexpr std::array numbers{ 4, 1, 8, 3, 2 };

TEST(SortFilterTest, HasTotalNumberOfItemsEqualToSizeOfSpanPassedToConstructor) {
    ASSERT_EQ(SortFilter<int>{ numbers }.totalItemCount(), numbers.size());
}

TEST(SortFilterTest, ItemsPassFilterAndPreserveOrderAfterConstruction) {
    const std::vector<int> items = ranges::view::values(SortFilter<int>{ numbers }.getItems()) | ranges::to<std::vector>();
    ASSERT_THAT(items, testing::ElementsAreArray(numbers));
}

TEST(SortFilterTest, FilteringExcludesItemsNotSatisfyingPredicate) {
    SortFilter<int> sortFilter{ numbers };
    sortFilter.filter([](int number) { return number % 2 != 0; });
    const std::vector<int> items = ranges::view::values(sortFilter.getItems()) | ranges::to<std::vector>();
    ASSERT_THAT(items, testing::ElementsAre(1, 3));
}

}
