#include "catch.hpp"

#include "../Osiris/InventoryChanger/ItemGenerator.cpp"

TEST_CASE("getTournamentMatches() invalid tournament ID") {
    REQUIRE(getTournamentMatches(0).empty());
    REQUIRE(getTournamentMatches(999).empty());
}

TEST_CASE("getTournamentMatches() valid tournament ID") {
    REQUIRE_FALSE(getTournamentMatches(1).empty());

    for (std::size_t i = 3; i <= 14; ++i)
        REQUIRE_FALSE(getTournamentMatches(i).empty());
}

TEST_CASE("tmToUTCTimestamp()") {
    const auto min = getStartOfYearTimestamp(2015);
    const auto max = getEndOfYearTimestamp(2050);

    for (std::size_t i = 0; i < 1000; ++i) {
        const auto timestamp = Helpers::random(min, max);
        auto tm = *std::gmtime(&timestamp);
        REQUIRE(tmToUTCTimestamp(tm) == timestamp);
    }
}

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

TEST_CASE("getEndOfYearTimestamp()") {
    REQUIRE(getEndOfYearTimestamp(2015) == 1451606399);
    REQUIRE(getEndOfYearTimestamp(2016) == 1483228799);
    REQUIRE(getEndOfYearTimestamp(2017) == 1514764799);
    REQUIRE(getEndOfYearTimestamp(2018) == 1546300799);
    REQUIRE(getEndOfYearTimestamp(2019) == 1577836799);
    REQUIRE(getEndOfYearTimestamp(2020) == 1609459199);
    REQUIRE(getEndOfYearTimestamp(2021) == 1640995199);
    REQUIRE(getEndOfYearTimestamp(2022) == 1672531199);
    REQUIRE(getEndOfYearTimestamp(2023) == 1704067199);
    REQUIRE(getEndOfYearTimestamp(2024) == 1735689599);
    REQUIRE(getEndOfYearTimestamp(2025) == 1767225599);
}

TEST_CASE("getRandomDateTimestampOfYear() for past years") {
    for (std::uint16_t year = 2015; year <= 2020; ++year) {
        const auto startOfYearTimestamp = getStartOfYearTimestamp(year);
        const auto endOfYearTimestamp = getEndOfYearTimestamp(year);

        for (std::size_t i = 0; i < 1000; ++i) {
            const auto timestamp = getRandomDateTimestampOfYear(year);
            REQUIRE((timestamp >= startOfYearTimestamp && timestamp <= endOfYearTimestamp));
        }
    }
}
