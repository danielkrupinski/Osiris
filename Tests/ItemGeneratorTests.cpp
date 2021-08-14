#include "catch.hpp"

#include "../Osiris/InventoryChanger/ItemGenerator.cpp"

TEST_CASE("getStartOfYearTimestamp()") {
    REQUIRE(getStartOfYearTimestamp(2015) == 1420070400);
    REQUIRE(getStartOfYearTimestamp(2016) == 1451606400);
    REQUIRE(getStartOfYearTimestamp(2017) == 1483228800);
    REQUIRE(getStartOfYearTimestamp(2018) == 1514764800);
    REQUIRE(getStartOfYearTimestamp(2019) == 1546300800);
    REQUIRE(getStartOfYearTimestamp(2020) == 1577836800);
    REQUIRE(getStartOfYearTimestamp(2021) == 1609459200);
    REQUIRE(getStartOfYearTimestamp(2022) == 1640995200);
    REQUIRE(getStartOfYearTimestamp(2023) == 1672531200);
    REQUIRE(getStartOfYearTimestamp(2024) == 1704067200);
    REQUIRE(getStartOfYearTimestamp(2025) == 1735689600);
}
