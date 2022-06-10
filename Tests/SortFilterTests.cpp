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
    const std::vector<int> items = ranges::views::values(SortFilter<int>{ numbers }.getItems()) | ranges::to<std::vector>();
    ASSERT_THAT(items, testing::ElementsAreArray(numbers));
}

TEST(SortFilterTest, FilteringExcludesItemsNotSatisfyingPredicate) {
    SortFilter<int> sortFilter{ numbers };
    sortFilter.filter([](int number) { return number % 2 != 0; });
    const std::vector<int> items = ranges::views::values(sortFilter.getItems()) | ranges::to<std::vector>();
    ASSERT_THAT(items, testing::ElementsAre(1, 3));
}

TEST(SortFilterTest, FilterIsPreservedAfterSorting) {
    SortFilter<int> sortFilter{ numbers };
    sortFilter.filter([](int number) { return number % 2 == 0; });
    sortFilter.sort(std::less{});
    const std::vector<int> values = ranges::views::values(sortFilter.getItems()) | ranges::to<std::vector>();
    ASSERT_THAT(values, testing::ElementsAre(2, 4, 8));
}

struct NonCopyableNonMovable {
    constexpr NonCopyableNonMovable(int value) : value{ value } {}

    NonCopyableNonMovable(NonCopyableNonMovable&&) = delete;
    NonCopyableNonMovable(const NonCopyableNonMovable&) = delete;
    NonCopyableNonMovable& operator=(NonCopyableNonMovable&&) = delete;
    NonCopyableNonMovable& operator=(const NonCopyableNonMovable&) = delete;

    int value;
};

class SortFilter_NonCopyableNonMovableTest : public testing::Test {
protected:
    static constexpr std::array<NonCopyableNonMovable, 4> objects{ 1, 3, 4, 2 };
};

TEST_F(SortFilter_NonCopyableNonMovableTest, CanBeFiltered) {
    SortFilter<NonCopyableNonMovable> sortFilter{ objects };
    sortFilter.filter([](const auto& object) { return object.value > 2; });
    const std::vector<int> values = sortFilter.getItems() |
                                    ranges::views::values |
                                    ranges::views::transform(&NonCopyableNonMovable::value) |
                                    ranges::to<std::vector>();
    ASSERT_THAT(values, testing::ElementsAre(3, 4));
}

TEST_F(SortFilter_NonCopyableNonMovableTest, CanBeSorted) {
    SortFilter<NonCopyableNonMovable> sortFilter{ objects };
    sortFilter.sort([](const auto& a, const auto& b) { return a.value > b.value; });
    const std::vector<int> values = sortFilter.getItems() |
                                    ranges::views::values |
                                    ranges::views::transform(&NonCopyableNonMovable::value) |
                                    ranges::to<std::vector>();
    ASSERT_THAT(values, testing::ElementsAre(4, 3, 2, 1));
}

}
