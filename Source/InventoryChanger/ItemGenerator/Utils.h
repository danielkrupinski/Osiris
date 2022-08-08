#pragma once

#include <Helpers.h>

namespace inventory_changer::item_generator
{

enum SkinCondition {
    FactoryNew = 1,
    MinimalWear,
    FieldTested,
    WellWorn,
    BattleScarred
};

[[nodiscard]] inline std::pair<std::time_t, std::time_t> clampTimespanToNow(std::time_t min, std::time_t max) noexcept
{
    const auto now = std::time(nullptr);
    return std::make_pair((std::min)(min, now), (std::min)(max, now));
}

[[nodiscard]] inline std::uint32_t getRandomDateTimestampOfYear(std::uint16_t year) noexcept
{
    const auto [min, max] = clampTimespanToNow(getStartOfYearTimestamp(year), getEndOfYearTimestamp(year));
    return static_cast<std::uint32_t>(Helpers::random(min, max));
}

}
