#include <gtest/gtest.h>

#include "../../Osiris/InventoryChanger/ItemGenerator.h"

struct TimeParam {
    std::uint16_t year;
    std::time_t expectedTimestamp;

    friend std::ostream& operator<<(std::ostream& os, const TimeParam& param)
    {
        return os << param.year;
    }
};

class GetStartOfYearTimestampTest : public testing::TestWithParam<TimeParam> {};

TEST_P(GetStartOfYearTimestampTest, ReturnsExpectedValue) {
    EXPECT_EQ(getStartOfYearTimestamp(GetParam().year), GetParam().expectedTimestamp);
}

INSTANTIATE_TEST_SUITE_P(, GetStartOfYearTimestampTest, testing::Values(
    TimeParam{ 2015, 1420070400 },
    TimeParam{ 2016, 1451606400 },
    TimeParam{ 2017, 1483228800 },
    TimeParam{ 2018, 1514764800 },
    TimeParam{ 2019, 1546300800 },
    TimeParam{ 2020, 1577836800 },
    TimeParam{ 2021, 1609459200 },
    TimeParam{ 2022, 1640995200 },
    TimeParam{ 2023, 1672531200 },
    TimeParam{ 2024, 1704067200 },
    TimeParam{ 2025, 1735689600 }
));

class GetEndOfYearTimestampTest : public testing::TestWithParam<TimeParam> {};

TEST_P(GetEndOfYearTimestampTest, ReturnsExpectedValue) {
    EXPECT_EQ(getEndOfYearTimestamp(GetParam().year), GetParam().expectedTimestamp);
}

INSTANTIATE_TEST_SUITE_P(, GetEndOfYearTimestampTest, testing::Values(
    TimeParam{ 2015, 1451606399 },
    TimeParam{ 2016, 1483228799 },
    TimeParam{ 2017, 1514764799 },
    TimeParam{ 2018, 1546300799 },
    TimeParam{ 2019, 1577836799 },
    TimeParam{ 2020, 1609459199 },
    TimeParam{ 2021, 1640995199 },
    TimeParam{ 2022, 1672531199 },
    TimeParam{ 2023, 1704067199 },
    TimeParam{ 2024, 1735689599 },
    TimeParam{ 2025, 1767225599 }
));
