#include <gtest/gtest.h>
#include <vector>

#include <Utils/SpanSlice.h>

namespace
{

TEST(SpanSliceTest, SliceSizeIsClampedBySpanSize) {
    SpanSlice<10, int> slice{ {}, 123 };
    EXPECT_TRUE(slice().empty());
}

TEST(SpanSliceTest, SliceIsCreatedAroundCenterOffsetIfPossible) {
    std::vector<int> v(1000);
    SpanSlice<200, int> slice{ v, 256 };
    EXPECT_EQ(&slice().front(), &v[156]);
    EXPECT_EQ(&slice().back(), &v[355]);
}

TEST(SpanSliceTest, SliceIsCreatedFromStartIfCenterOffsetIsLowerThanHalfOfSliceSize) {
    std::vector<int> v(500);
    SpanSlice<200, int> slice{ v, 80 };
    EXPECT_EQ(&slice().front(), &v[0]);
    EXPECT_EQ(&slice().back(), &v[199]);
}

TEST(SpanSliceTest, SliceIsCreatedBeforeEndIfCenterOffsetIsHigherThanSpanSizeMinusHalfOfSliceSize) {
    std::vector<int> v(500);
    SpanSlice<200, int> slice{ v, 490 };
    EXPECT_EQ(&slice().front(), &v[300]);
    EXPECT_EQ(&slice().back(), &v[499]);
}

}
