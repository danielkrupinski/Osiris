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

inline SkinCondition randomSkinCondition()
{
    if (const auto condition = Helpers::random(1, 10000); condition <= 1471)
        return FactoryNew;
    else if (condition <= 3939)
        return MinimalWear;
    else if (condition <= 8257)
        return FieldTested;
    else if (condition <= 9049)
        return WellWorn;
    return BattleScarred;
}

inline float generateWear(SkinCondition condition)
{
    static constexpr auto wearRanges = std::to_array<float>({ 0.0f, 0.07f, 0.15f, 0.38f, 0.45f, 1.0f });
    return Helpers::random(wearRanges[condition - 1], wearRanges[condition]);
}

inline float generateWear()
{
    return generateWear(randomSkinCondition());
}

[[nodiscard]] inline int randomSeed()
{
    return Helpers::random(1, 1000);
}

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
