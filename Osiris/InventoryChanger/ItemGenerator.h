#pragma once

#include <cstddef>
#include <ctime>
#include <utility>

#include "StaticData.h"

struct InventoryItem;

namespace ItemGenerator
{
    std::pair<const game_items::GameItem&, std::size_t> generateItemFromContainer(const InventoryItem& caseItem) noexcept;
    std::size_t createDefaultDynamicData(const game_items::GameItem& item) noexcept;
}

[[nodiscard]] std::time_t tmToUTCTimestamp(std::tm& tm) noexcept;
[[nodiscard]] std::time_t getStartOfYearTimestamp(std::uint16_t year) noexcept;
[[nodiscard]] std::time_t getEndOfYearTimestamp(std::uint16_t year) noexcept;
